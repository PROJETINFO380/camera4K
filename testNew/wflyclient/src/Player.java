//package player;

import java.awt.Point;
import java.awt.Transparency;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.ComponentColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferByte;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import javax.imageio.ImageIO;


//fournis par Monsieur Nicolas Bouch¨¦
public class Player implements ActionListener{

	    VideoPlayer player = new VideoPlayer();
        
        public void openJavaWindow() {
        
                player.addWindowListener(new WindowAdapter() {
                        public void windowClosing(WindowEvent we) {
                                closeJavaWindow();
                        }
                });
        }

        public void updateJavaWindow(BufferedImage javaImage) {
                player.setImage(javaImage);
                
        }


        public void closeJavaWindow() {
                //this.stopCapture(reference);
                System.exit(0);
        }

		@Override
		public void actionPerformed(ActionEvent arg0) {
			// TODO Auto-generated method stub
			
		}
		
		protected static BufferedImage getImage(byte[] rawImage, int width, int height) throws IOException {
			//width = 4096;
			//height = 2160;

	        DataBuffer buffer = new DataBufferByte(rawImage, rawImage.length);

	        // 3 bytes per pixel: red, green, blue
	        WritableRaster raster = Raster.createInterleavedRaster(buffer, width,
	                        height, 3 * width, 3, new int[] { 2, 1, 0 }, (Point) null);
	        /*WritableRaster raster = Raster.createInterleavedRaster(buffer, width,
                    height,  width, 1, new int[] { 0 }, (Point) null);*/
	        ColorModel cm = new ComponentColorModel(ColorModel.getRGBdefault()
	                        .getColorSpace(), false, true, Transparency.OPAQUE,
	                        DataBuffer.TYPE_BYTE);
	        BufferedImage picture = new BufferedImage(cm, raster, true, null);
	        
	        return picture;
	}

		
	/*	private void play (Player player) throws IOException{
			//reference = player.
			openJavaWindow();
			byte[] rawPicture = null;
			int count = 0;
			while(true){
				rawPicture= api.capture();
				if(rawPicture!=null){
					BufferedImage picture = getImage(rawPicture);
					updateJavaWindow(picture);
					count++;
				}
			}
			closeJavaWindow();
		}
		*/
		
}

