import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Point;
import java.awt.Transparency;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.ComponentColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferByte;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.io.IOException;

import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.SwingUtilities;
import javax.swing.WindowConstants;

public class VideoPlayer extends JFrame {

	private static final long serialVersionUID = 6157707834200365458L;
	private final ImageComponent screenImage;
	
	//set le frame et loader les images
	//Auteur: Xiaofei
	public VideoPlayer()
    {
		JFrame f;
		this.screenImage = new ImageComponent();
		JFrame.setDefaultLookAndFeelDecorated(true);
		JDialog.setDefaultLookAndFeelDecorated(true);
		f=new JFrame("player");
		f.setSize(4096,2160);//set la taille du frame

		f.setLocationRelativeTo(null);
	    Container cp=f.getContentPane();
	    JScrollPane jsp=new JScrollPane(this.screenImage);
	    cp.add(jsp,BorderLayout.CENTER);//loader les images
		
        f.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
		f.setVisible(true);
		pack();
	}
	

	public void setImage(BufferedImage aImage)
    {
		this.screenImage.setImage(aImage);
	}

	
	//fournis par Monsieur Bouch¨¦
	public class ImageComponent extends JComponent
    {

		private static final long serialVersionUID = -7472451187140864307L;
		private Image myImage;
		private Dimension mySize;

		public void setImage(Image image) {
			SwingUtilities.invokeLater(new ImageRunnable(image));
		}

		public void setImageSize(Dimension newSize) {
		}

		private class ImageRunnable implements Runnable {
			private final Image newImage;
		
			public ImageRunnable(Image newImage) {
				this.newImage = newImage;
			}

			public void run() {
				VideoPlayer.ImageComponent.this.myImage = this.newImage;
				Dimension newSize = new Dimension(
						VideoPlayer.ImageComponent.this.myImage.getWidth(null),
						VideoPlayer.ImageComponent.this.myImage.getHeight(null));
				if (!newSize.equals(VideoPlayer.ImageComponent.this.mySize)) {
					VideoPlayer.ImageComponent.this.mySize = newSize;
					VideoPlayer.this.setSize(
							VideoPlayer.ImageComponent.this.myImage
									.getWidth(null),
							VideoPlayer.ImageComponent.this.myImage
									.getHeight(null));
					VideoPlayer.this.setVisible(true);
				}
				VideoPlayer.ImageComponent.this.repaint();
			}
		}

		public ImageComponent() {
			this.mySize = new Dimension(0, 0);
			setSize(this.mySize);
		}

		public synchronized void paint(Graphics g) {
			if (this.myImage != null) {
				g.drawImage(this.myImage, 0, 0, this);
			}
		}
	}
}
