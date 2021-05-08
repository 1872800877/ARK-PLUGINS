package cn.lyjdwz.server.exception;

public class MiraiServerOfflineException extends RuntimeException{
	private static final long serialVersionUID = 2184819203031027193L;

	public MiraiServerOfflineException(String message) {
        super(message);
    }

    public MiraiServerOfflineException(String message, Throwable cause) {
        super(message, cause);
    }
}