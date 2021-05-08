package cn.lyjdwz.server;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.Set;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.alibaba.fastjson.JSONObject;

import cn.lyjdwz.server.utils.JsonConfigUtils;
import cn.lyjdwz.server.utils.MiraiServerUtils;
import cn.lyjdwz.server.vo.MiraiServerEnity;
import cn.lyjdwz.server.vo.MiraiServerQueueEntry;
import cn.lyjdwz.server.vo.ProcessQueueRunable;
import cn.lyjdwz.server.vo.ServerThreadFactory;

public class MiraiServer {
	private static final int DEFAUT_PORT = 8888;
	private static Logger logger = LoggerFactory.getLogger(MiraiServer.class);
	private static Selector selector;
	private static ServerSocketChannel serverSocketChannel;
	private static ThreadPoolExecutor executorService = new ThreadPoolExecutor(3, 5, 200, TimeUnit.MILLISECONDS,
			new ArrayBlockingQueue<Runnable>(5),new ServerThreadFactory());
	private static LinkedBlockingQueue<MiraiServerQueueEntry> writeQueue = new LinkedBlockingQueue<MiraiServerQueueEntry>();

	public static void main(String[] args) {
		try {
			selector = Selector.open();
			serverSocketChannel = ServerSocketChannel.open();
			JSONObject botConfig = JsonConfigUtils.getServerConfig();
			int port = botConfig.getIntValue("port");
			String url = botConfig.getString("url");
			serverSocketChannel.socket().bind(new InetSocketAddress(url, port > 0 ? port : DEFAUT_PORT));
			serverSocketChannel.configureBlocking(false);
			serverSocketChannel.register(selector, SelectionKey.OP_ACCEPT);
			logger.info("socket服务启动成功");
			logger.info("端口为:"+port);
		} catch (IOException e) {
			logger.error("服务器启动失败:", e);
			return;
		}
		ProcessQueueRunable processQueueRunable = new ProcessQueueRunable(selector, writeQueue);
		executorService.submit(processQueueRunable);
		while (true) {
			try {
				if (selector.select() > 0) {
					Set<SelectionKey> keys = selector.selectedKeys();
					Iterator<SelectionKey> it = keys.iterator();
					while (it.hasNext()) {
						SelectionKey key = it.next();
						it.remove();
						if (key.isAcceptable()) {
							SocketChannel socketChannel = serverSocketChannel.accept();
							socketChannel.configureBlocking(false);
							socketChannel.register(selector, SelectionKey.OP_READ);
							logger.info("socket连接成功：ip" + socketChannel.getRemoteAddress());
						} else if (key.isReadable()) {
							processRead(key);
						}
					}
				}
			} catch (Exception e) {
				logger.error("socket服务器select处理失败：", e);
			}
		}
	}

	/**
	 * 读取客户端消息
	 * 
	 * @param key
	 */
	private static void processRead(SelectionKey key) {
		key.interestOps(key.interestOps() & ~key.readyOps());
		SocketChannel channel = (SocketChannel) key.channel();
		try {
			executorService.submit(new Runnable() {
				@Override
				public void run() {
					try {
						MiraiServerEnity enity = MiraiServerUtils.processReadResponse(channel);
						key.interestOps(key.interestOps()| key.readyOps());
						selector.wakeup();
						MiraiServerQueueEntry attachment = new MiraiServerQueueEntry();
						attachment.setEnity(enity);
						attachment.setPort(channel.socket().getPort());
						writeQueue.add(attachment);
						logger.info("服务器收到" + attachment.getPort() + "信息：" + enity.getStringData());
					} catch (Exception e) {
						try {
							logger.error(channel.getRemoteAddress().toString() + " is offline", e);
							// 关闭通道
							channel.close();
						} catch (Exception ex) {
							logger.error("读取客户端消息关闭失败", ex);
						}
					}
				}
			});
		} catch (Exception e) {
			logger.error("提交线程失败", e);
		}
	}
}
