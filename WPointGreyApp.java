public class WPointGreyApp
{
  static{
     System.loadLibrary("wpointgrey");
  }
 
  private native void initContext();
  private native void connect();
  
  private native int getNumCamera();
  private native void printCamera();
  private native void disconnect();
  private native void startCapture();
  private native void stopCapture();
  
  private native void destroysContext();
  

  
  

  public static void main(String []args)
  {
    WPointGreyApp c  = new WPointGreyApp();
    c.initContext();
	c.connect();
	c.printCamera();
	c.startCapture();
	c.stopCapture();
	c.disconnect();
	c.destroysContext(); 
  }
 
}







