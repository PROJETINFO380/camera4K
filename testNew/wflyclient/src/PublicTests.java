import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import javax.imageio.ImageIO;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import junit.framework.TestCase;
import it.sauronsoftware.jave.*;

/**
 * Ce test est utilis¨¦ pour tester les fonctionnalit¨¦s de cam¨¦ra quand il est connect?
 * Auteurs: Yuqi, Jin, Biyun
 */
public class PublicTests extends TestCase {
	static{
		System.loadLibrary("wpointgrey");
	}
	 
    WPointGreyApp api = new WPointGreyApp();
    
    //initialiser la camera
    @Before
    public void setUp(){
    	api.initContext();
    	api.connect();
    	api.startCamera();
    }
    
    @After
	public void tearDown(){
    	api.stopCapture();
		api.disconnect();
	}
	
    //v¨¦rifier le nombre de cam¨¦ra
	@Test
	public void testNumbreCamera(){
        assertEquals(1,api.getNumCamera());
    }

	//v¨¦rifier la taille de l'image enregistr¨¦e
	@Test
    public void testTaillePhoto(){
		File picture = new File("test.png");
		BufferedImage sourceImg = null;
		try {
			sourceImg = ImageIO.read(new FileInputStream(picture));
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		assertEquals(4096,sourceImg.getWidth());
		assertEquals(2160,sourceImg.getHeight());
    }

	//v¨¦rifier le framerate de la vid¨¦o enregistr¨¦e
	@Test
    public void testFrameRate(){
	
		File source = new File("test.mp4");
		Encoder encoder = new Encoder();
		//float fr = api.getFramerate();
		
		try {
			MultimediaInfo m = encoder.getInfo(source);
			long ls = m.getDuration();
			if((ls/1000 < 100/21*1.1)&&(ls/1000 > 100/21*0.9)){
				assertEquals(1,1);
			}
			else{
				assertEquals(1,2);
			}
			
		} catch (InputFormatException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (EncoderException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	       
    }

	//v¨¦rifier le height de l'image
	@Test
	public void testPictureHeight(){
		//System.out.println("please choose 1:YUV and enter 4096*2160");
		//api.calibrageFormat();
		assertEquals(2160,api.getHeight());
   }

	//v¨¦rifier le width de l'image
	@Test
	public void testPictureWidth(){
		//System.out.println("please choose 1:YUV and enter 4096*2160");
		//api.calibrageFormat();
		assertEquals(4096,api.getWidth());
	}
   
	//v¨¦rifier le framerate(valeur de setting == valeur de getting)
	@Test
	public void testFramerate(){
		//api.setCustomerFrameRate();
		float fr = (float)21.0;
		assertEquals(fr,api.getFramerate());
	}
   
	//v¨¦rifier le framerate du player(vrai framerate)
	@Test
	public void testFrameratePlayer(){
		Player player = new Player();
		//byte rawimage[] = api.capture();
		player.openJavaWindow();
		byte[] rawPicture = null;
		int count = 0;
		
		//api.setCustomerFrameRate();
		long start_time = System.nanoTime();
		while(count<100){
			rawPicture= api.capture();
			if(rawPicture!=null){
			BufferedImage picture;
			try {
				int width = api.getWidth();
				int height = api.getHeight();
				float framerate = api.getFramerate();
				
				picture = player.getImage(rawPicture, width, height);
				//player.updateJavaWindow(picture);
				System.out.println(framerate);
				
				count++;
				//System.out.println(rawPicture);
				
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			}
	   }
	   long end_time = System.nanoTime();
	   double difference = end_time - start_time;//en nanoseconde
	   float framerate_actuel = (float) (100/(difference*1e-9));
	   
	   System.out.println(framerate_actuel);
	   if((framerate_actuel<api.getFramerate()*1.1)&&(framerate_actuel>api.getFramerate()*0.9)){
		   assertEquals(1,1);
	   }
	   else{
		   assertEquals(1,0);
	   }
   }
}