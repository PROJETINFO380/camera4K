/*
 * Ce fichier est utilisé pour lancer l'application directement
 * Auteurs: Xiaofei, Pelagie, Jin, Yuqi, Biyun 
 */
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Paths;
import java.util.Scanner ;
public class ClientJavaPointGray
{
	static{
		System.loadLibrary("wpointgrey");
	}
	
	public static void main (String [] args ){
		WPointGreyApp api = new WPointGreyApp(); 
		api.initContext();
		api.connect();
		api.startCamera();
		api.calibrageFormat();
		api.setCustomerFrameRate();
		api.brightness();
		api.WBRedBlue();
		
		Player player = new Player();
		//byte rawimage[] = api.capture();
		player.openJavaWindow();
        byte[] rawPicture = null;
        int count = 0;
	
        while(true){
			rawPicture= api.capture();
			if(rawPicture!=null){
			BufferedImage picture;
			try {
				int width = api.getWidth();
				int height = api.getHeight();
				float framerate = api.getFramerate();
				//System.out.println(height+" "+width);
				picture = player.getImage(rawPicture, width, height);
				player.updateJavaWindow(picture);
				System.out.println(framerate);
				count++;
				//System.out.println(rawPicture);
				
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			}

		}
	}
	
}