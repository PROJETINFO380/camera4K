#include <stdio.h>
#include <stdlib.h>

//notre point h
#include "WPointGreyApp.h"

//inclusion des entetes de pointgray
#include  <C/FlyCapture2_C.h>

// definition des constantes
#define PIXEL_FORMAT 3
#define WIDTH 4096
#define HEIGHT 2160
#define FRAMERATE 21
#define BRIGHTNESS 5
#define WHITEBALANCE_RED 550
#define WHITEBALANCE_BLUE 750

//variables globales
fc2Context pContext ;
fc2PGRGuid pGuid ;
fc2AVIContext pAVIContext ;

typedef enum _AviType
{
	UNCOMPRESSED,
	MJPG,
	H264
} AviType;

/*
    luminosité:	[0..255]
	W.B red:	[0..1023], avec un booléen on/off, on == active , off = desactive
    W.B blue:	[0..1023]
	- pour que ça marche, il ne faut pas que la camera soit dans le mode absolute
*/

/*
 * Auteurs: Pelagie, Xiaofei
 */
void SetTimeStamping( fc2Context context, BOOL enableTimeStamp )
{
    fc2Error error;
    fc2EmbeddedImageInfo embeddedInfo;

    //exemple d'appel de la fonction de la dll
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


/*
 * setting le format(format de l'image, taille de l'image)
 * Auteurs: Yuqi, Jin, Biyun
 */
void Format(fc2Context context, int fmt7PixFmt, int width, int height)
{
	fc2Mode k_fmt7Mode = FC2_MODE_0;
	fc2PixelFormat k_fmt7PixFmt;
	fc2Error error;

    //setting le format de l'image
	switch(fmt7PixFmt)
	{
	case 1:
		k_fmt7PixFmt = FC2_PIXEL_FORMAT_422YUV8;
		break;
	case 2:
		k_fmt7PixFmt = FC2_PIXEL_FORMAT_RGB8;
		break;
	default:
		//printf("commande nom prise en charge / inconnu !!");
		k_fmt7PixFmt = FC2_PIXEL_FORMAT_RAW8;
		break;
	}


	// Query for available Format 7 modes
	fc2Format7Info fmt7Info;
	BOOL supported;
	fmt7Info.mode = k_fmt7Mode;
	error = fc2GetFormat7Info(pContext, &fmt7Info, &supported);
	if(error != FC2_ERROR_OK)
	{
		printf( "Error in fc2GetFormatInfo: %s\n", fc2ErrorToDescription( error) );
		return;
	}

	if ( (k_fmt7PixFmt & fmt7Info.pixelFormatBitField) == 0 )
	{
		// Pixel format not supported!
		printf( "Pixel format is not supported" );
	    return;
	}

    //setting la taille de l'image
	fc2Format7ImageSettings fmt7ImageSettings;
	fmt7ImageSettings.mode = k_fmt7Mode;
	fmt7ImageSettings.offsetX = 0;
	fmt7ImageSettings.offsetY = 0;
	if (width> fmt7Info.maxWidth)
	{
		printf("Error Width");
	}
	if (height> fmt7Info.maxWidth)
	{
		printf("Error height");
	}
	fmt7ImageSettings.width = width;
	fmt7ImageSettings.height = height;
	fmt7ImageSettings.pixelFormat = k_fmt7PixFmt;

	BOOL valid;
	fc2Format7PacketInfo fmt7PacketInfo;

	// Valider les settings
	error = fc2ValidateFormat7Settings(
			pContext,
			&fmt7ImageSettings,
			&valid,
	        &fmt7PacketInfo );
	if(error != FC2_ERROR_OK)
	{
		printf( "Error in fc2ValidateFormat7Settings: %s\n", fc2ErrorToDescription( error) );
		return;
	}

	if( !valid )
	{
		// Settings invalides
		printf( "Format7 settings are not valid" );
	    return;
	}

	// Set les settings à la caméra
	error = fc2SetFormat7ConfigurationPacket(
			pContext,
	        &fmt7ImageSettings,
	        fmt7PacketInfo.recommendedBytesPerPacket );
	if (error != FC2_ERROR_OK)
	{
		printf( "Error in fc2SetFormat7ConfigurationPacket: %s\n", fc2ErrorToDescription( error) );
		return;
	}

}


/*
 * capturer les images
 * Auteurs: Pelagie, Xiaofei
 */
void GrabImages( fc2Context context, int numImagesToGrab, const char * fileName )
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
			int diff = (ts.cycleSeconds - prevTimestamp.cycleSeconds) * 8000 + (ts.cycleCount - prevTimestamp.cycleCount);
			prevTimestamp = ts;
/*			printf(
					"timestamp [%d %d] - %d\n",
					ts.cycleSeconds,
					ts.cycleCount,
					diff );*/
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
//		printf("Saving the last image to %s \n",fileName);
		error = fc2SaveImage( &convertedImage, fileName, FC2_PNG );
		if ( error != FC2_ERROR_OK )
		{
			printf( "Error in fc2SaveImage: %s\n", fc2ErrorToDescription( error) );
			printf( "Please check write permissions.\n");
		}
	}
	error = fc2DestroyImage( &rawImage );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2DestroyImage: %s\n", fc2ErrorToDescription( error) );
	}
	error = fc2DestroyImage( &convertedImage );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2DestroyImage: %s\n", fc2ErrorToDescription( error) );
	}
}

/*
 * Auteurs: Biyun, Yuqi, Jin
 */
float GetFrameRate(fc2Context pContext)
{
	fc2Error error;
	fc2PropertyInfo propInfo;
	fc2Property prop;
	// Check if the camera supports the FRAME_RATE property
//	printf( "Detecting frame rate from camera... \n" );
	propInfo.type = FC2_FRAME_RATE;
	error = fc2GetPropertyInfo(pContext, &propInfo);
	if (error != FC2_ERROR_OK)
	{
		return 0.0f;
	}
	if (propInfo.present)
	{
		// Get the frame rate
		prop.type = FC2_FRAME_RATE;
		error = fc2GetProperty(pContext, &prop);
		if (error != FC2_ERROR_OK)
		{
			return 0.0f;
		}
		return prop.absValue;
	}
	return 0.0f;
}


/*
 * sauvegarder la video
 * Auteurs: Pelagie, Xiaofei
 */
int SaveAVIHelper(fc2Context context, AviType aviType, float frameRate, int count, const char* fileName)
{
	fc2Error error;
	fc2Image rawImage;
	fc2AVIContext aviContext;
	fc2AVIOption aviOption;
	fc2H264Option h264Option;
	fc2MJPGOption mjpgOption;
	int i;
	error = fc2CreateAVI(&aviContext);
	if (error != FC2_ERROR_OK)
	{
		printf("Error in fc2CreateAVI: %d\n", error);
		return -1;
	}
	error = fc2CreateImage( &rawImage );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2CreateImage: %d\n", error );
		fc2DestroyAVI(aviContext);
		return -1;
	}
	for (i=0; i < count; i++)
	{
		// Retrieve the image
		error = fc2RetrieveBuffer(context, &rawImage);
		if (error != FC2_ERROR_OK)
		{
			printf("Error in retrieveBuffer: %d\n", error);
			continue;
		}
		// Perform some initialization for the first time
		if (i == 0)
		{
			switch (aviType)
			{
				case H264:
					h264Option.frameRate = frameRate;
					h264Option.bitrate = 1000000;
					h264Option.width = rawImage.cols;
					h264Option.height = rawImage.rows;
					error = fc2H264Open(aviContext, "SaveImageToAviEx_C-H264", &h264Option);
					if (error != FC2_ERROR_OK)
					{
						printf("Error opening AVI: %d\n", error);
					}
					break;
			}
		}
		error = fc2AVIAppend(aviContext, &rawImage);
		if (error != FC2_ERROR_OK)
		{
			printf("Error appending to AVI: %d\n", error);
		}
//		printf("Appended image %d\n", i);
	}
	error = fc2DestroyImage(&rawImage);
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2CreateImaged: %d\n", error );
		fc2DestroyAVI(aviContext);
		return -1;
	}
	error = fc2DestroyAVI(aviContext);
	if (error != FC2_ERROR_OK)
	{
		printf("Error in fc2DestroyAVI: %d\n", error);
		return -1;
	}
	return 0;
}


/*
 * Auteurs: Pelagie, Xiaofei
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
 * Auteurs:  Pelagie, Xiaofei
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
 * Auteurs:  Pelagie, Xiaofei
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

    propInfo.type = FC2_WHITE_BALANCE ;
	error = fc2GetPropertyInfo( pContext,&propInfo );


	printf ("  present : %d\n",propInfo.present)	;
	printf ("  autoSupported : %d\n",propInfo.autoSupported)	;
	printf ("  manualSupported : %d\n",propInfo.manualSupported)	;
	printf ("  onePushSupported : %d\n",propInfo.onePushSupported)	;
	printf ("  absValSupported : %d\n",propInfo.absValSupported)	;
	printf ("  min value: %d\n",propInfo.min)	;
	printf ("  max value : %d\n",propInfo.max)	;

	//on essaie d'augmenter la luminosité
/*
	fc2Property prop ;
	prop.type=FC2_WHITE_BALANCE;
	error = fc2GetProperty( pContext,&prop );
	prop.absValue=1023;
	fc2SetProperty( pContext,&prop  );
*/
}


/*
 * Auteurs: Biyun, Yuqi, Jin
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


/*
 * Auteurs: Xiaofei, Pelagie
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
 * Auteurs: Xiaofei, Pelagie
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
 * Auteurs: Xiaofei, Pelagie
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_startCapture
(JNIEnv *env, jobject obj, jint count, jstring fileName){
/*	SetTimeStamping( pContext, TRUE );
	fc2Error error = fc2StartCapture( pContext );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2StartCapture: %s\n", fc2ErrorToDescription( error) );
		return ;
	}*/
	const char *inCStr = (*env)->GetStringUTFChars(env,fileName, 0);
	GrabImages( pContext, (int)count ,inCStr );
	(*env)->ReleaseStringUTFChars(env,fileName,inCStr);
}

/*
 * Auteurs: Xiaofei, Pelagie
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_startVideo
(JNIEnv *env, jobject jobject, jint count, jstring fileName){
	fc2Error error;
/*	error = fc2StartCapture( pContext );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2StartCapture: %s\n", fc2ErrorToDescription( error) );
		return ;
	}
	*/
	float frameRate = 0.0f;
	frameRate = GetFrameRate(pContext);
	if (frameRate == 0.0f)
	{
		printf("Invalid frame rate returned\n");
		return ;
	}
	SaveAVIHelper(pContext, H264, frameRate,count, "");
}

/*
 * Auteurs: Xiaofei, Pelagie
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_startCamera
  (JNIEnv *env, jobject obj){

	SetTimeStamping( pContext, TRUE );
	fc2Error error = fc2StartCapture( pContext );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2StartCapture: %s\n", fc2ErrorToDescription( error) );
		return ;
	}
  }


/*
 * Auteurs: Xiaofei, Pelagie
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
 * Auteurs: Yuqi, Jin, Biyun
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_calibrageFormat
  (JNIEnv *env, jobject obj){
/*
	int format;		//printf("Choose the format: 1 422YUV 2 RGB\n");
	scanf("%d", &format);
	int width;		//printf("enter the width:\n");
	scanf("%d", &width);
	int height;		//printf("enter the height:\n");
	scanf("%d", &height);
*/
	fc2Error error = fc2StopCapture( pContext );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2StopCapture: %s\n", fc2ErrorToDescription( error) );
        return ;
    }

	Format(pContext, PIXEL_FORMAT, WIDTH, HEIGHT);

	SetTimeStamping( pContext, TRUE );
	error = fc2StartCapture( pContext );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2StartCapture: %s\n", fc2ErrorToDescription( error) );
		return ;
	}
  }

/*
 * Auteurs: Biyun, Yuqi, Jin
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_setVideoModeAndFrameRate
(JNIEnv *env, jobject obj){

	fc2Error error;
	fc2VideoMode videoMode;
	videoMode = FC2_VIDEOMODE_1600x1200YUV422;
	fc2FrameRate frameRate;
	int i = 0;

//	float frameRate = 0.0f;
//	printf("set framerate... \n");
	scanf("%d", &i);

	switch(i)
    {
		case 1:
                frameRate = FC2_FRAMERATE_1_875;
                //printf("framerate 1.875 fps\n");
                break;
        case 2:
                frameRate = FC2_FRAMERATE_3_75;
                //printf("framerate 3.75 fps\n");
                break;
		case 3:
                frameRate = FC2_FRAMERATE_15;
                //printf("framerate 15 fps\n");
                break;
		default:
                frameRate = FC2_FRAMERATE_15;
                //printf("framerate 15 fps\n");

    }

	error = fc2StopCapture( pContext );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2StopCapture: %s\n", fc2ErrorToDescription( error) );
        return ;
    }

	error=fc2SetVideoModeAndFrameRate(pContext,videoMode,frameRate);
	if ( error != FC2_ERROR_OK )
    {
        printf( "Error in SetVideoModeAndFrameRate: %d\n", error );
        return;
    }

	SetTimeStamping( pContext, TRUE );
	error = fc2StartCapture( pContext );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2StartCapture: %s\n", fc2ErrorToDescription( error) );
		return ;
	}

}

/*
 * Auteurs: Jin, Biyun, Yuqi
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_brightness
  (JNIEnv *env, jobject obj){

    fc2Error error;
	fc2Property propInfo;

	error = fc2StopCapture( pContext );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2StopCapture: %s\n", fc2ErrorToDescription( error) );
        return ;
    }

    propInfo.type = FC2_BRIGHTNESS;
	error = fc2GetProperty( pContext,&propInfo );
/*	printf ("  present : %d\n",propInfo.present)	;
	printf ("  absControl : %d\n",propInfo.absControl)	;
	printf ("  onePush : %d\n",propInfo.onePush)	;
	printf ("  onOff : %d\n",propInfo.onOff)	;
	printf ("  autoManualMode : %d\n",propInfo.autoManualMode)	;
	printf("%f\n", propInfo.absValue);
	*/
	propInfo.absControl = 1;

/*	float brightness = propInfo.absValue;
	scanf("%f", &brightness);*/
	propInfo.absValue = BRIGHTNESS;

	error = fc2SetProperty(pContext,&propInfo);
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2SetProperty: %s\n", fc2ErrorToDescription( error) );
		return ;
	}

	SetTimeStamping( pContext, TRUE );
	error = fc2StartCapture( pContext );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2StartCapture: %s\n", fc2ErrorToDescription( error) );
		return ;
	}

  }

/*
 * Auteurs: Biyun, Yuqi, Jin
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_setCustomerFrameRate
  (JNIEnv *env, jobject obj){

    fc2Error error;
	fc2Property propInfo;

	error = fc2StopCapture( pContext );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2StopCapture: %s\n", fc2ErrorToDescription( error) );
        return ;
    }

    propInfo.type = FC2_FRAME_RATE;
	error = fc2GetProperty( pContext,&propInfo );
/*	printf ("  present : %d\n",propInfo.present)	;
	printf ("  absControl : %d\n",propInfo.absControl)	;
	printf ("  onePush : %d\n",propInfo.onePush)	;
	printf ("  onOff : %d\n",propInfo.onOff)	;
	printf ("  autoManualMode : %d\n",propInfo.autoManualMode)	;
	printf("%f\n", propInfo.absValue);*/

	propInfo.absControl = 1;
	propInfo.autoManualMode = 0;

/*	float framerate = propInfo.absValue;
	scanf("%f", &framerate);*/
	propInfo.absValue = FRAMERATE;

	error = fc2SetProperty(pContext,&propInfo);
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2SetProperty: %s\n", fc2ErrorToDescription( error) );
		return ;
	}

	SetTimeStamping( pContext, TRUE );
	error = fc2StartCapture( pContext );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2StartCapture: %s\n", fc2ErrorToDescription( error) );
		return ;
	}

  }

/*
 * Auteurs: Jin, Biyun, Yuqi
 */
JNIEXPORT void JNICALL Java_WPointGreyApp_WBRedBlue
  (JNIEnv *env, jobject obj){

    fc2Error error;
	fc2Property propInfo;

	error = fc2StopCapture( pContext );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2StopCapture: %s\n", fc2ErrorToDescription( error) );
        return ;
    }

    propInfo.type = FC2_WHITE_BALANCE;
	error = fc2GetProperty( pContext,&propInfo );

//	int wbred = propInfo.valueA;
//	scanf("%d", &wbred);
	propInfo.valueA = WHITEBALANCE_RED;

//	int wbblue = propInfo.valueB;
//	scanf("%d", &wbblue);
	propInfo.valueB = WHITEBALANCE_BLUE;

	error = fc2SetProperty(pContext,&propInfo);
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2SetProperty: %s\n", fc2ErrorToDescription( error) );
		return ;
	}

	SetTimeStamping( pContext, TRUE );
	error = fc2StartCapture( pContext );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2StartCapture: %s\n", fc2ErrorToDescription( error) );
		return ;
	}

  }

 /*
 * Auteurs: Xiaofei, Pelagie
 */
JNIEXPORT jbyteArray JNICALL Java_WPointGreyApp_capture(JNIEnv *env, jobject obj){
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
	for ( i=0; i < 1; i++ )
	{
	// Retrieve the image
		error = fc2RetrieveBuffer( pContext, &rawImage );
		if ( error != FC2_ERROR_OK )
		{
			printf( "Error in retrieveBuffer: %s\n", fc2ErrorToDescription( error));
		}
		else
		{
			// Get and print out the time stamp
			fc2TimeStamp ts = fc2GetImageTimeStamp(&rawImage);
			int diff = (ts.cycleSeconds - prevTimestamp.cycleSeconds) * 8000 + (ts.cycleCount - prevTimestamp.cycleCount);
			prevTimestamp = ts;
/*			printf(
					"timestamp [%d %d] - %d\n",
					ts.cycleSeconds,
					ts.cycleCount,
					diff );*/
		}
	}

	fc2Format7ImageSettings imageSettings;
    unsigned int packetSize;
    float percentage;
	fc2Mode k_fmt7Mode = FC2_MODE_0;
	imageSettings.mode = k_fmt7Mode;

	error = fc2GetFormat7Configuration( pContext,&imageSettings,&packetSize,&percentage);
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2GetFormat7Configuration: %s\n", fc2ErrorToDescription( error) );
	}


	if(imageSettings.pixelFormat == FC2_PIXEL_FORMAT_422YUV8){

			char* output = rawImage.pData;
			int len = rawImage.dataSize;

			jbyteArray jbyteArr;
			jbyteArr = (*env)->NewByteArray(env,len);

			jbyte* b = (jbyte*)output;

			(*env)->SetByteArrayRegion(env, jbyteArr, 0, len, b);

			error = fc2DestroyImage( &rawImage );
			if ( error != FC2_ERROR_OK )
			{
				printf( "Error in fc2DestroyImage: %s\n", fc2ErrorToDescription( error) );
			}

			error = fc2DestroyImage( &convertedImage );
			if ( error != FC2_ERROR_OK )
			{
				printf( "Error in fc2DestroyImage: %s\n", fc2ErrorToDescription( error) );
			}

			return jbyteArr;
		}

		else{
			if ( error == FC2_ERROR_OK )
			{
				// Convert the final image to RGB
				error = fc2ConvertImageTo(FC2_PIXEL_FORMAT_BGR, &rawImage, &convertedImage);
				if ( error != FC2_ERROR_OK )
				{
					printf( "Error in fc2ConvertImageTo: %s\n", fc2ErrorToDescription( error) );
				}
			}
			error = fc2DestroyImage( &rawImage );
			if ( error != FC2_ERROR_OK )
			{
				printf( "Error in fc2DestroyImage: %s\n", fc2ErrorToDescription( error) );
			}

			char* output = convertedImage.pData;
			int len = convertedImage.dataSize;

			jbyteArray jbyteArr;
			jbyteArr = (*env)->NewByteArray(env,len);

			jbyte* b = (jbyte*)output;

			(*env)->SetByteArrayRegion(env, jbyteArr, 0, len, b);

			error = fc2DestroyImage( &convertedImage );
			if ( error != FC2_ERROR_OK )
			{
				printf( "Error in fc2DestroyImage: %s\n", fc2ErrorToDescription( error) );
			}

			return jbyteArr;
	}

}

/*
 * Auteurs: Jin
 */
JNIEXPORT jint JNICALL Java_WPointGreyApp_getHeight
  (JNIEnv *env, jobject obj){
    fc2Format7ImageSettings imageSettings;
    unsigned int packetSize;
    float percentage;
	fc2Mode k_fmt7Mode = FC2_MODE_0;
	imageSettings.mode = k_fmt7Mode;

	fc2Error error;
	error = fc2GetFormat7Configuration( pContext,&imageSettings,&packetSize,&percentage);
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2GetFormat7Configuration: %s\n", fc2ErrorToDescription( error) );
	}

	jint jheight = (jint)imageSettings.height;

	return jheight;
  }

/*
 * Auteurs: Yuqi
 */
JNIEXPORT jint JNICALL Java_WPointGreyApp_getWidth
  (JNIEnv *env, jobject obj){
	fc2Format7ImageSettings imageSettings;
    unsigned int packetSize;
    float percentage;
	fc2Mode k_fmt7Mode = FC2_MODE_0;
	imageSettings.mode = k_fmt7Mode;

	fc2Error error;
	error = fc2GetFormat7Configuration( pContext,&imageSettings,&packetSize,&percentage);
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2GetFormat7Configuration: %s\n", fc2ErrorToDescription( error) );
	}

	jint jwidth = (jint)imageSettings.width;

	return jwidth;

  }

/*
 * Auteurs: Biyun
 */
JNIEXPORT jfloat JNICALL Java_WPointGreyApp_getFramerate
  (JNIEnv *env, jobject obj){
	fc2Error error;
	fc2Property propInfo;

    propInfo.type = FC2_FRAME_RATE;
	error = fc2GetProperty( pContext,&propInfo );
	if ( error != FC2_ERROR_OK )
	{
		printf( "Error in fc2GetProperty: %s\n", fc2ErrorToDescription( error) );
	}

	float framerate = propInfo.absValue;
	jfloat jframerate = (jfloat)framerate;

	return jframerate;

  }
