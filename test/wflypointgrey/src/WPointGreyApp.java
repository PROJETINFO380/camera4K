


/**
  API JAVA de capture d'image avec point gray
*/

public class WPointGreyApp
{      
  public native void initContext();
  public native void connect();
  public native int getNumCamera();
  public native void printCamera();
  public native void disconnect();
  public native void startCapture(int count, String imageName);
  public native void startVideo(int count, String imageName);
  public native void stopCapture();
  public native void calibrageCouleur();
  public native void destroysContext();
}   







