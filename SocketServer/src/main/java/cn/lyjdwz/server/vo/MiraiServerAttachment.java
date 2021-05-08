package cn.lyjdwz.server.vo;

import com.alibaba.fastjson.JSONObject;

public class MiraiServerAttachment {
	private MiraiServerEnity enity;
	private JSONObject jsonObject;
	public MiraiServerEnity getEnity() {
		return enity;
	}
	public void setEnity(MiraiServerEnity enity) {
		this.enity = enity;
	}
	public JSONObject getJsonObject() {
		return jsonObject;
	}
	public void setJsonObject(JSONObject jsonObject) {
		this.jsonObject = jsonObject;
	}
	
}
