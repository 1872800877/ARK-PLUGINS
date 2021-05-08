package cn.lyjdwz.server.vo;

public class MiraiServerQueueEntry {
	private MiraiServerEnity enity;
	private int port;
	private String name;
	public MiraiServerEnity getEnity() {
		return enity;
	}
	public void setEnity(MiraiServerEnity enity) {
		this.enity = enity;
	}
	public int getPort() {
		return port;
	}
	public void setPort(int port) {
		this.port = port;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
}
