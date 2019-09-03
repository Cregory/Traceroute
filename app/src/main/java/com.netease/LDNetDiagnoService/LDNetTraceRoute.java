package com.netease.LDNetDiagnoService;

/**
 * 通过ping模拟traceroute过程
 * 
 * @author panghui
 * 
 */
public class LDNetTraceRoute {
  static {
    try {
      System.loadLibrary("tracert");
    } catch (UnsatisfiedLinkError e) {
      e.printStackTrace();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  private Callback mCallback;

  public LDNetTraceRoute(Callback callback){
    this.mCallback = callback;
  }

  public native void beginTrace(String path);

  public void callback(String update) {
    if (null != mCallback) {
      mCallback.onUpdate(update);
    }
  };

  public void end() {
    if (null != mCallback) {
      mCallback.onEnd();
    }
  }

  public  interface Callback{
    void onUpdate(String update);

    void onEnd();
  }
}
