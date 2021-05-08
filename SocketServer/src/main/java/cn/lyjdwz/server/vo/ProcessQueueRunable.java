package cn.lyjdwz.server.vo;

import java.nio.channels.Channel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.util.concurrent.LinkedBlockingQueue;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import cn.lyjdwz.server.utils.MiraiServerUtils;

public class ProcessQueueRunable implements Runnable{
	Logger logger = LoggerFactory.getLogger(getClass());
	private LinkedBlockingQueue<MiraiServerQueueEntry> writeQueue;
	private Selector selector;
	public ProcessQueueRunable(Selector selector,LinkedBlockingQueue<MiraiServerQueueEntry> writeQueue) {
		this.selector = selector;
		this.writeQueue = writeQueue;
	}
	@Override
	public void run() {
		while(true) {
			try {
				MiraiServerQueueEntry entry = writeQueue.take();
				MiraiServerEnity enity = entry.getEnity();
				for (SelectionKey key : selector.keys()) {
					// 通过key取出对应的SocketChannel
					Channel targetChannel = key.channel();
					// 排除自己
					if (targetChannel instanceof SocketChannel
							&& ((SocketChannel) targetChannel).socket().getPort() != entry.getPort()) {
						// 转型
						SocketChannel dest = (SocketChannel) targetChannel;
						// 将buffer数据写入到通道
						synchronized (dest) {
							try {
								dest.write(MiraiServerUtils.toByteBuffer(enity));
							} catch (Exception e) {
								logger.error("发送消息错误", e);
								try {
									// 取消注册
									key.cancel();
									// 关闭通道
									dest.close();
								} catch (Exception ex) {
									logger.error("发送消息关闭失败", ex);
								}
							}
						}
					}
				}
			} catch (InterruptedException e) {
				logger.error("线程池阻塞失败", e);
			}
		}
	}
}
