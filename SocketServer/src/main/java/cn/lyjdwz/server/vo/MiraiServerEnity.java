package cn.lyjdwz.server.vo;

import java.nio.charset.StandardCharsets;

public class MiraiServerEnity {
	private int size;
	private int type;
	private byte[] data;
	public int getSize() {
		return size;
	}
	public void setSize(int size) {
		this.size = size;
	}
	public int getType() {
		return type;
	}
	public void setType(int type) {
		this.type = type;
	}
	public byte[] getData() {
		return data;
	}
	public void setData(byte[] data) {
		this.data = data;
	}
	public String getStringData() {
		return new String(data,StandardCharsets.UTF_8);
	}
}
