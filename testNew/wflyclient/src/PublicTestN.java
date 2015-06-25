import org.junit.Test;

import junit.framework.TestCase;

/**
 * Ce test est utilis¨¦ pour v¨¦rifier le nombre de cam¨¦ra quand il n'y pas de cam¨¦ra connect¨¦.
 * Auteurs: Jin, Yuqi, Biyun
 */
public class PublicTestN extends TestCase {
	static{
		System.loadLibrary("wpointgrey");
		}
	WPointGreyApp api = new WPointGreyApp();
	
	@Test
	public void testNumbreCamera() {	
	    //getNumberCamera();
        assertEquals(0,api.getNumCamera());
    }
	
/*	@Test//(expected = Exception.class)
	public void testNoNumbreCamera() {
        assertEquals(2,2);
    }
*/
}