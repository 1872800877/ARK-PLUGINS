/**
* @Title: JsonConfigUtils.java
* @Package com.mirai.bean
* @Description: TODO(用一句话描述该文件做什么)
* @author LiuYuJin
* @date 2020年12月10日
* @version V1.0
*/
package cn.lyjdwz.server.utils;

import com.alibaba.fastjson.JSONObject;

/**
* @ClassName: JsonConfigUtils
* @Description: TODO(这里用一句话描述这个类的作用)
* @author LiuYuJin
* @date 2020年12月10日
*
*/

public class JsonConfigUtils {
	private static final JsonConfigUtils instance = new JsonConfigUtils();
	private static final String SERVER_CONFIG = "server";
	private JSONObject config;
	private JsonConfigUtils() {
		config = JsonFileUtils.getJsonObjectFormPath("serverConfig.json");
	}
	private static JSONObject getInstance() {
		return instance.config;
	}
	public static JSONObject getServerConfig() {
		return getInstance().getJSONObject(SERVER_CONFIG);
	}
}
