package cn.lyjdwz.server.utils;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

import cn.lyjdwz.server.exception.MiraiServerException;
import cn.lyjdwz.server.exception.MiraiServerOfflineException;
import cn.lyjdwz.server.vo.MiraiServerEnity;

public class MiraiServerUtils {
	private static final Charset PAYLOAD_CHARSET_UTF8 = StandardCharsets.UTF_8;
	
	public static MiraiServerEnity processReadResponse(SocketChannel socketChannel){
		MiraiServerEnity enity = new MiraiServerEnity();
        int size = readData(Integer.BYTES,socketChannel).getInt();
        int type = readData(Integer.BYTES,socketChannel).getInt();
        ByteBuffer dataBuffer = readData(size-(2*Integer.BYTES)-(2 * Byte.BYTES),socketChannel);
        ByteBuffer nullsBuffer = readData(2 * Byte.BYTES,socketChannel);
        byte null1 = nullsBuffer.get(0);
        byte null2 = nullsBuffer.get(1);
        if (null1 != 0 || null2 != 0) {
            throw new MiraiServerException("Expected 2 null bytes but received " + null1 + " and " + null2);
        }
        enity.setSize(size);
        enity.setType(type);
        enity.setData(dataBuffer.array());
        return  enity;
    }

	public static ByteBuffer readData(int size,SocketChannel socketChannel){
        ByteBuffer buffer = ByteBuffer.allocate(size);
        int readCount=0;
        try {
        	while (readCount < size) {
        		readCount +=socketChannel.read(buffer);
    		}
        } catch (IOException e) {
            throw new MiraiServerOfflineException("已离线", e);
        }
        if (readCount != size) {
            throw new MiraiServerException("Expected " + size + " bytes but received " + readCount);
        }
        buffer.order(ByteOrder.LITTLE_ENDIAN) ;
        buffer.flip();
        return buffer;
    }
    
    public static ByteBuffer toByteBuffer(int type, String payload) {
    	byte[] bytes = payload.getBytes(PAYLOAD_CHARSET_UTF8);
        ByteBuffer buffer = ByteBuffer.allocate((2 * Integer.BYTES) + bytes.length+(2 * Byte.BYTES));
        buffer.order(ByteOrder.LITTLE_ENDIAN) ;
        buffer.putInt((2 * Integer.BYTES) + bytes.length+(2 * Byte.BYTES));
        buffer.putInt(type);
        buffer.put(bytes);
        buffer.put((byte) 0);
        buffer.put((byte) 0);
        buffer.flip();
        return buffer;
    }
    public static ByteBuffer toByteBuffer(MiraiServerEnity enity) {
        ByteBuffer buffer = ByteBuffer.allocate(enity.getSize());
        buffer.order(ByteOrder.LITTLE_ENDIAN) ;
        buffer.putInt(enity.getSize());
        buffer.putInt(enity.getType());
        buffer.put(enity.getData());
        buffer.put((byte) 0);
        buffer.put((byte) 0);
        buffer.flip();
        return buffer;
    }
}
