#include <stdio.h>
#include <stdlib.h>
#include "WPointGreyApp.h"
#include  <C/FlyCapture2_C.h>


#define WHITEBALANCE 3
#define BRIGHTNESS 0
fc2Context pContext ;
fc2PGRGuid pGuid ;

/*
    - luminosité :  [0..255]
	 W.B red   : [0..1023] , avec un booléen on/off, on == active , off = desactive
     W.B blue   : [0..1023]
	- pour que ça marche, il ne faut pas que la camera soit dans le mode absolute
*/
	

void SetTimeStamping( fc2Context context, BOOL enableTimeStamp )
{
    fc2Error error;
    fc2EmbeddedImageInfo embeddedInfo;

    error = fc2GetEmbeddedImageInfo( context, &embeddedInfo );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2GetEmbeddedImageInfo: %s\n", fc2ErrorToDescription( error) );
    }

    if ( embeddedInfo.timestamp.available != 0 )
    {       
        embeddedInfo.timestamp.onOff = enableTimeStamp;
    }    

    error = fc2SetEmbeddedImageInfo( context, &embeddedInfo );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2SetEmbeddedImageInfo: %s\n", fc2ErrorToDescription( error) );
    }
}


void GrabImages( fc2Context context, int numImagesToGrab )
{
    fc2Error error;
    fc2Image rawImage;
    fc2Image convertedImage;
    fc2TimeStamp prevTimestamp = {0};
    int i;

    error = fc2CreateImage( &rawImage );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2CreateImage: %s\n", fc2ErrorToDescription( error) );
    }

    error = fc2CreateImage( &convertedImage );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2CreateImage: %s\n", fc2ErrorToDescription( error) );
    }

    // If externally allocated memory is to be used for the converted image,
    // simply assigning the pData member of the fc2Image structure is
    // insufficient. fc2SetImageData() should be called in order to populate
    // the fc2Image structure correctly. This can be done at this point,
    // assuming that the memory has already been allocated.

    for ( i=0; i < numImagesToGrab; i++ )
    {
        // Retrieve the image
        error = fc2RetrieveBuffer( context, &rawImage );
        if ( error != FC2_ERROR_OK )
        {
            printf( "Error in retrieveBuffer: %s\n", fc2ErrorToDescription( error));
        }
        else
        {
            // Get and print out the time stamp
            fc2TimeStamp ts = fc2GetImageTimeStamp( &rawImage);
            int diff = (ts.cycleSeconds - prevTimestamp.cycleSeconds) * 8000
                        + (ts.cycleCount - prevTimestamp.cycleCount);
            prevTimestamp = ts;
            printf( 
                "timestamp [%d %d] - %d\n", 
                ts.cycleSeconds, 
                ts.cycleCount, 
                diff );
        }       
    }

    if ( error == FC2_ERROR_OK )
    {
        // Convert the final image to RGB
        error = fc2ConvertImageTo(FC2_PIXEL_FORMAT_BGR, &rawImage, &convertedImage);
        if ( error != FC2_ERROR_OK )
        {
            printf( "Error in fc2ConvertImageTo: %s\n", fc2ErrorToDescription( error) );
        }

        // Save it to PNG
        printf("Saving the last image to mapo.png \n");
		error = fc2SaveImage( &convertedImage, "mapo.png", FC2_PNG );
		if ( error != FC2_ERROR_OK )
		{
			printf( "Error in fc2SaveImage: %s\n", fc2ErrorToDescription( error) );
			printf( "Please check write permissions.\n");
		}
    }

    error = fc2DestroyImage( &rawImage );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2DestroyImage: %d\n", fc2ErrorToDescription( error) );
    }

    error = fc2DestroyImage( &convertedImage );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2DestroyImage: %s\n", fc2ErrorToDescription( error) );
    }
}

	
	/*
 * Class:     WPointGreyApp
 * Method:    initContext
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_initContext
  (JNIEnv *en, jobject obj){
   printf("initializing context \n");
   fc2Error err = fc2CreateContext(&pContext );
   
   if ( err != FC2_ERROR_OK )
    {
        printf( "Error in fc2CreateContext: %d\n", fc2ErrorToDescription( err) );
        exit(-1) ;
    }  
   err=fc2GetCameraFromIndex(pContext,0,&pGuid );

    if ( err != FC2_ERROR_OK )
    {
        printf( "Error in fc2GetNumOfCameras: %s\n", fc2ErrorToDescription( err) );
        exit(-1) ;
    }

  }

  
  
  /*
 * Class:     WPointGreyApp
 * Method:    startCapture
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_startCapture
  (JNIEnv *env, jobject obj){
   SetTimeStamping( pContext, TRUE );
   fc2Error error = fc2StartCapture( pContext );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2StartCapture: %s\n", fc2ErrorToDescription( error) );
        return ;
    }

    GrabImages( pContext, 1 ); 
  }
  
  

/*
 * Class:     WPointGreyApp
 * Method:    stopCapture
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_stopCapture
  (JNIEnv *env, jobject obj){
  
 fc2Error error = fc2StopCapture( pContext );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2StopCapture: %s\n", fc2ErrorToDescription( error) );
        return ;
    }
  }
  
  
  /*
 * Class:     WPointGreyApp
 * Method:    connect
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_connect
  (JNIEnv *env, jobject obj){
  fc2Error err = fc2Connect( pContext, &pGuid );
    if ( err != FC2_ERROR_OK )
    {
        printf( "Error in fc2Connect: %s\n", fc2ErrorToDescription( err) );
        exit(-1) ;
    }
  
  }
  
  /*
 * Class:     WPointGreyApp
 * Method:    disconnect
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_disconnect
  (JNIEnv *env, jobject obj){
  fc2Error error = fc2Disconnect( pContext );
	
	 if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2Disconnect: %s\n", fc2ErrorToDescription( error)
		);
        return ;
    }

  }
  
  /*
 * Class:     WPointGreyApp
 * Method:    destroysContext
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_destroysContext
  (JNIEnv *env, jobject obj){
   printf("destroying context \n");
   fc2Error error = fc2DestroyContext( pContext );  
   if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2DestroyContext: %s\n", fc2ErrorToDescription( error) );
        return ;
    }
  }
/*
 * Class:     WPointGreyApp
 * Method:    printCamera
 * Signature: ()I
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_printCamera
  (JNIEnv *env, jobject obj){
    fc2PropertyInfo propInfo ;
    fc2Error error;
    fc2CameraInfo camInfo;
    error = fc2GetCameraInfo( pContext, &camInfo );
    if ( error != FC2_ERROR_OK )
    {
	   printf( "Error in fc2GetCameraInfo: %s\n", fc2ErrorToDescription( error) );
        return ;
    }

    printf(
        "\n*** CAMERA INFORMATION ***\n"
        "Serial number - %u\n"
        "Camera model - %s\n"
        "Camera vendor - %s\n"
        "Sensor - %s\n"
        "Resolution - %s\n"
        "Firmware version - %s\n"
        "Firmware build time - %s\n\n",
        camInfo.serialNumber,
        camInfo.modelName,
        camInfo.vendorName,
        camInfo.sensorInfo,
        camInfo.sensorResolution,
        camInfo.firmwareVersion,
        camInfo.firmwareBuildTime );
		
	 // on extrait le propriété pour voir ce qu'il y a dedans
/*
typedef enum _fc2PropertyType
{
    FC2_BRIGHTNESS,
    FC2_AUTO_EXPOSURE,
    FC2_SHARPNESS,
    FC2_WHITE_BALANCE,
    FC2_HUE,
    FC2_SATURATION,
    FC2_GAMMA,
    FC2_IRIS,
    FC2_FOCUS,
    FC2_ZOOM,
    FC2_PAN,
    FC2_TILT,
    FC2_SHUTTER,
    FC2_GAIN,
    FC2_TRIGGER_MODE,
    FC2_TRIGGER_DELAY,
    FC2_FRAME_RATE,
    FC2_TEMPERATURE,
    FC2_UNSPECIFIED_PROPERTY_TYPE,
    FC2_PROPERTY_TYPE_FORCE_32BITS = FULL_32BIT_VALUE

} fc2PropertyType;

*/
    propInfo.type = FC2_WHITE_BALANCE ; 	 
	 error = fc2GetPropertyInfo( pContext,&propInfo );	
	 
	 /*
	 typedef struct _Property
		{
			fc2PropertyType   type;
			BOOL present;
			BOOL absControl;
			BOOL onePush;
			BOOL onOff;
			BOOL autoManualMode;
			unsigned int valueA;  
			unsigned int valueB; //Applies only to the white balance blue value. Use
			//Value A for the red value.
			float absValue;
			unsigned int reserved[8];

			// For convenience, trigger delay is the same structure
			// used in a separate function along with trigger mode.

		} fc2Property, fc2TriggerDelay;
	
     typedef struct _fc2PropertyInfo
		{
			fc2PropertyType type;
			BOOL present;
			BOOL autoSupported;
			BOOL manualSupported;
			BOOL onOffSupported;
			BOOL onePushSupported;
			BOOL absValSupported;
			BOOL readOutSupported;
			unsigned int min;
			unsigned int max;
			float absMin;
			float absMax;
			char pUnits[MAX_STRING_LENGTH];
			char pUnitAbbr[MAX_STRING_LENGTH];
			unsigned int reserved[8];

		} fc2PropertyInfo, fc2TriggerDelayInfo; 	
		
	 */
	 printf ("  present : %d\n",propInfo.present)	;
	  printf ("  autoSupported : %d\n",propInfo.autoSupported)	;
	   printf ("  manualSupported : %d\n",propInfo.manualSupported)	;
	    printf ("  onePushSupported : %d\n",propInfo.onePushSupported)	;
		 printf ("  absValSupported : %d\n",propInfo.absValSupported)	;
		  printf ("  min value: %d\n",propInfo.min)	;
		   printf ("  max value : %d\n",propInfo.max)	;
		
	//on essaie d'augmenter la luminosité

	fc2Property prop ;
	prop.type=FC2_WHITE_BALANCE;
	error = fc2GetProperty( pContext,&prop ); 
	prop.absValue=1023;
	fc2SetProperty( pContext,&prop  );

}
/*
 * Class:     WPointGreyApp
 * Method:    getNumCamera
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_WPointGreyApp_getNumCamera
  (JNIEnv *env, jobject obj) {
  
   fc2Context pContext ;
   fc2Error err = fc2CreateContext(&pContext );
   unsigned int pNumCameras = 0 ;
   err=fc2GetNumOfCameras(pContext,&pNumCameras);
   err=fc2DestroyContext( pContext );  
   jint result = (jint)pNumCameras;
   return result ;
  
}


