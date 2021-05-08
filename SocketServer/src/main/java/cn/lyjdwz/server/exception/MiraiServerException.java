package cn.lyjdwz.server.exception;

public class MiraiServerException extends RuntimeException{
	private static final long serialVersionUID = 6569783892652081067L;
	public MiraiServerException(String message) {
        super(message);
    }

    public MiraiServerException(String message, Throwable cause) {
        super(message, cause);
    }
}
