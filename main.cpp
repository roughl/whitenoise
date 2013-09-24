#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>
#include <thread>
#include <memory>
#include <fstream> //read the urandom and random device
#include <time.h> //use the "second" as seed generator if the random devices do fail
#include <sys/stat.h> //for create directory
#include <signal.h> //for ctrl+c to abort
#include <tclap/CmdLine.h>
#include <jpeglib.h>
#include <unistd.h>
#include "PerlinNoise.h"
#include "WhiteNoise.h"
#include "NormalNoise.h"
 
int beenden = 0; //Program flow

unsigned int generateSeed();
void catcher(int sigtype);
bool write_jpeg_file( char *filename, unsigned int width, unsigned int height, unsigned char* data );

unsigned int surfaceWidth, surfaceHeight;

void generate_pictures(unsigned int folder, std::string compression,
		INoise *mynoiser, INoise *mynoiseg, INoise *mynoiseb, unsigned int start, unsigned int stop) {

	SDL_Surface *surface = NULL;
	// @TODO endian issues
	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, surfaceWidth, surfaceHeight, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0);
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);
	for(unsigned int j=start; j<stop; j++)
	{
		Uint8 *ptr;
		ptr = (Uint8 *)surface->pixels;
		//cout << "\r " << j+1 << "/" <<200<<std::flush;
		for(unsigned int y=0; y<surfaceHeight; y++) {
			for(unsigned int x=0; x<surfaceWidth; x++) {
				int index = (y*surfaceWidth+x)*3;
				//ptr[index]   = mynoiser->getNoise(x/150.0,y/150.0,(j/100.));
				//ptr[index+1] = mynoiseg->getNoise(x/150.0,y/150.0,(j/100.));
				//ptr[index+2] = mynoiseb->getNoise(x/150.0,y/150.0,(j/100.));
				ptr[index]   = mynoiser->getNoise(x/(j+1.0),y/(j+1.0),(1));
				ptr[index+1] = mynoiseg->getNoise(x/(j+1.0),y/(j+1.0),(1));
				ptr[index+2] = mynoiseb->getNoise(x/(j+1.0),y/(j+1.0),(1));
			}
		}

		char filename[32];
		sprintf(filename,"%.3d/%.3d.%s",folder,j,compression.c_str());
		if(compression == "bmp")
			SDL_SaveBMP(surface, filename);
		else if(compression == "jpg")
			write_jpeg_file( filename, surface->w, surface->h, (unsigned char*)surface->pixels);

		if (beenden==1)
			break;
	}
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
	SDL_FreeSurface(surface);
}


int main(int argc, char* args[])
{
	using namespace std;
	string compression;
	string noise;
	unsigned int threads, folderNumber, npics;

	signal(SIGINT, catcher);

	try
	{
		TCLAP::CmdLine cmd("Creates pictures with random content", ' ', "0.1");
		TCLAP::ValueArg<unsigned int> widthArg("x", "width", "Width in pixels", false, 960, "unsigned int");
		TCLAP::ValueArg<unsigned int> heightArg("y", "height", "Height in pixels", false, 960, "unsigned int");
		TCLAP::ValueArg<unsigned int> foldersArg("f", "folders", "Number of folders you'd like to fill", false, 1, "unsigned int");
		TCLAP::ValueArg<unsigned int> pictureArg("p", "pictures", "Number of pictures per folder you'd like to create", false, 200, "unsigned int");
		TCLAP::ValueArg<unsigned int> threadsArg("t", "threads", "Number of threads you'd like to use", false, 1, "unsigned int");
		vector<string> compressionList;
		compressionList.push_back("bmp");
		compressionList.push_back("jpg");
		TCLAP::ValuesConstraint<string> compressionMethods( compressionList );
		TCLAP::ValueArg<string> compressionArg("c", "compression", "Type of compression to use", false, "bmp", &compressionMethods);

		vector<string> noiseList;
		noiseList.push_back("white");
		noiseList.push_back("perlin");
		noiseList.push_back("normal");
		TCLAP::ValuesConstraint<string> noiseFunctions( noiseList );
		TCLAP::ValueArg<string> noiseArg("n", "noise", "Type of noise to use", false, "white", &noiseFunctions);
		
		cmd.add(widthArg);
		cmd.add(heightArg);
		cmd.add(foldersArg);
		cmd.add(pictureArg);
		cmd.add(threadsArg);
		cmd.add(compressionArg);
		cmd.add(noiseArg);

		cmd.parse(argc, args);

		surfaceWidth  = widthArg.getValue();
		surfaceHeight = heightArg.getValue();
		folderNumber  = foldersArg.getValue();
		threads       = threadsArg.getValue();
		npics         = pictureArg.getValue();
		compression   = compressionArg.getValue();
		noise         = noiseArg.getValue();
	}
	catch (TCLAP::ArgException & e)
	{
		std::cerr << "I don't understand " << e.argId() << ".\n" << e.error() << std::endl;
		return -1;
	}

	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
		return -1;

	INoise *mynoiser;
	INoise *mynoiseg;
	INoise *mynoiseb;
	if( noise == "perlin" ) {
		mynoiser = (INoise*)(new PerlinNoise(generateSeed()));
		mynoiseg = (INoise*)(new PerlinNoise(generateSeed()));
		mynoiseb = (INoise*)(new PerlinNoise(generateSeed()));
	}
	else if( noise == "white" ) {
		mynoiser = (INoise*)(new WhiteNoise(generateSeed()));
		mynoiseg = mynoiser;
		mynoiseb = mynoiser;
	}
	else if( noise == "normal" ) {
		mynoiser = (INoise*)(new NormalNoise(generateSeed()));
		mynoiseg = mynoiser;
		mynoiseb = mynoiser;
	}
	else {
		return -1;
	}

	mkdir("output",0755);
	chdir("output");
	for(unsigned int i=0; i<folderNumber; i++)
	{
		char foldername[32];
		sprintf(foldername,"%.3d",i);
		mkdir(foldername,0755);
		std::thread ts[threads];
		for(unsigned int t=0; t<threads; t++) {
			printf("%i,%i\n", npics*t/threads, npics*(t+1)/threads);
			ts[t] = std::thread(generate_pictures, i, compression, mynoiser, mynoiseg, mynoiseb, npics*t/threads, npics*(t+1)/threads);
		}
		for(unsigned int t=0; t<threads; t++) {
			ts[t].join();
		}
		if (beenden==1)
			break;
	}

	SDL_Quit();
}

unsigned int generateSeed()
{
	unsigned int seed, seedX, seedY, seedZ; 
	seedX = 0;
	seedY = 0;
	seedZ = time(0);

	char * tmp;
	const int size = sizeof(int);
	tmp = new char[size];

	using namespace std;
	ifstream file1("/dev/urandom", ios::binary);
	if(file1.is_open())
	{
		file1.read(tmp, size);
		file1.close();
		seedX = *reinterpret_cast<int*>(tmp);
	}

	ifstream file2("/dev/random", ios::binary);
	if(file2.is_open())
	{
		file2.read(tmp, size);
		file2.close();
		seedY = *reinterpret_cast<int*>(tmp);
	}

	delete[] tmp;
	seed = seedX xor seedY xor seedZ;
//	cout << "seed:" << seed << " seedX:" << seedX << " seedY:" << seedY << " seedZ:" << seedZ << endl;
	return seed;
}

void catcher(int sigtype)
{
	beenden=1;
}

/**
* write_jpeg_file writes the raw image data stored in the data buffer
* to a jpeg image with default compression and smoothing options in the file
* specified by filename. Source: http://www.aaronmr.com/en/2010/03/test/
*/
bool write_jpeg_file( char *filename, unsigned int width, unsigned int height, unsigned char* data )
{
	int bytes_per_pixel = 3;
	J_COLOR_SPACE color_space = JCS_RGB;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// this is a pointer to one row of image data
	JSAMPROW row_pointer[1];

	std::unique_ptr<FILE, int (*)(FILE *)> outfile(fopen(filename, "wb"), fclose);

	if ( !outfile )
	{
		fprintf(stderr, "Error opening output jpeg file %s\n!", filename );
		return false;
	}
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile.get());

	// Setting the parameters of the output file here
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = bytes_per_pixel;
	cinfo.in_color_space = color_space;
	// default compression parameters, we shouldn't be worried about these

	jpeg_set_defaults( &cinfo );
	cinfo.num_components = 3;
	//cinfo.data_precision = 4;
	cinfo.dct_method = JDCT_FLOAT;
	jpeg_set_quality(&cinfo, 100, TRUE);
	// Now do the compression
	jpeg_start_compress( &cinfo, TRUE );
	// like reading a file, this time write one row at a time
	while( cinfo.next_scanline < cinfo.image_height )
	{
		row_pointer[0] = &data[ cinfo.next_scanline * cinfo.image_width * cinfo.input_components];
		jpeg_write_scanlines( &cinfo, row_pointer, 1 );
	}
	// similar to read file, clean up after we're done compressing
	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
	
	return true;
}

