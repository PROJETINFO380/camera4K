
/**
  API JAVA de capture d'image avec point gray
  D¨¦clarer les m¨¦thodes natives JNI
*/

public class WPointGreyApp
{
  public native void initContext();
  public native void connect();
  public native int  getNumCamera();
  public native void printCamera();
  public native void disconnect();
  public native void startCapture(int count, String imageName);
  public native void stopCapture();
  public native void calibrageFormat();
  public native void destroysContext();
  public native void startVideo(int count, String imageName);
  public native void setVideoModeAndFrameRate();
  public native void brightness();
  public native void setCustomerFrameRate();
  public native void WBRedBlue();
  public native byte[] capture();
  public native void startCamera();
  public native int getHeight();
  public native int getWidth();
  public native float getFramerate();
}