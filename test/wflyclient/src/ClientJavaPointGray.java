

import java.nio.file.Paths;
import java.util.Scanner ;


public class ClientJavaPointGray 
{

  static{
     System.loadLibrary("wpointgrey");
  }

   public static void main (String [] args )
   {
   
        
        WPointGreyApp api  = new WPointGreyApp();
		//initialisation du context de la et detection possible pre. caméra branché.
		api.initContext();
		
		System.out.println("bienvenue dans le gestionnaire(demo) interactif de capture d'image ");
		
		System.out.println("commandes acceptés :\n\nconnect:\tse cnnecte a la camera \ndisconnect:\tse deconnecte de la camera \ninfo:\tafficher les informations  de la camera\nstart <count> <filename>:\tdemarre la capture\nvideo <count> <fichier> \n:stop:\tstop la capture\n\n");
		System.out.println("quit : pour sortir");
		
		Scanner clavier = new Scanner(System.in);
		
		
		String cmd = null;
		while ( !(cmd = clavier.nextLine()).startsWith("quit") ){ // sortir avec q, Q quit, Quit
		     String [] command = cmd.split(" "); 
			switch(command[0])
			   {
				
				 case "connect":
					   api.connect();
						  break ;
				 case "disconnect" :
					   api.disconnect();
					break ;
				 case "info" :
						api.printCamera();
						break;
				 case "start":
						if(command.length==3) {
						  try{
						     api.startCapture( Integer.parseInt( command[1]), command[2] );
						   } catch(NumberFormatException e){
						   
						    System.out.println("nombre d'images incorrecte");
						  }
						  
						}else{
						   System.out.println("format de la commande incorrecte");
						} 
						break ;
				  case "video":
						if(command.length==3) {
						  try{
						     api.startVideo( Integer.parseInt( command[1]), command[2] );
						   } catch(NumberFormatException e){
						   
						    System.out.println("nombre d'images incorrecte");
						  }
						  
						}else{
						   System.out.println("format de la commande incorrecte");
						} 
						break ;
						
				 case "stop" :
					 api.stopCapture();		 
				 break;
				 default :
				   System.out.println("commande nom prise en charge / inconnu !!");
			
			   }
			   
		    System.out.println("Entrer la commande : ");
	        System.out.println("commandes acceptés :\n\nconnect:\tse cnnecte a la camera \ndisconnect:\tse deconnecte de la camera \ninfo:\tafficher les informations  de la camera\nstart <count> <filename>:\tdemarre la capture\nvideo <count> <fichier>:capture video\nstop:\tstop la capture\n\n");
		    System.out.println("quit : pour sortir");
			
		}
	   api.destroysContext(); // destruction du context de la camera
	
   }
}
