/**
* @Title: JsonFileUtils.java
* @Package com.mirai.bean
* @Description: TODO(用一句话描述该文件做什么)
* @author LiuYuJin
* @date 2020年12月10日
* @version V1.0
*/
package cn.lyjdwz.server.utils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;

/**
* @ClassName: JsonFileUtils
* @Description: TODO(这里用一句话描述这个类的作用)
* @author LiuYuJin
* @date 2020年12月10日
*
*/
public class JsonFileUtils {
	public static JSONObject getJsonObjectFormPath(String path) {
		return getJsonObjectFormFile(new File(path));
	}
	public static JSONObject getJsonObjectFormFile(File file) {
		return JSONObject.parseObject(getJsonStringFromFile(file));
	}
	public static JSONArray getJsonArrayFormPath(String path) {
		return getJsonArrayFormFile(new File(path));
	}
	public static JSONArray getJsonArrayFormFile(File file) {
		return JSONArray.parseArray(getJsonStringFromFile(file));
	}
	public static String getJsonStringFromFile(File file) {
		StringBuffer stringBuffer = new StringBuffer();
		try {
			InputStream is = new FileInputStream(file);
	    	InputStreamReader inputStreamReader;
			inputStreamReader = new InputStreamReader(is, "UTF-8");
			BufferedReader reader = new BufferedReader(inputStreamReader);
	    	String tempString = null;
			while((tempString = reader.readLine()) != null){
				stringBuffer.append(tempString);
			}
			reader.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return stringBuffer.toString();
	}
}
