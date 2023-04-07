#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"



namespace fs = std::filesystem;


constexpr int WIDTH = 176;
constexpr int HEIGHT = 120;
const char* method;
const char* inputfile;
const char* outputfile;

//Encoding the file to a video
void encode(const char* inputfile, const char* outputfile)
{
	//Fancy print
	char result[1024];
	sprintf_s(result, sizeof(result), "Encoding: %s to: %s", inputfile, outputfile);
	std::cout << result << std::endl;

	//Get file contents
	std::ifstream input(inputfile, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

	std::cout << "Got file" << std::endl;

	// Convert binary buffer to array of bool values
	std::vector<bool> boolArray;
	for (std::vector<unsigned char>::const_iterator it = buffer.begin(); it != buffer.end(); ++it)
	{
		unsigned char byte = *it;
		for (int i = 7; i >= 0; i--)
		{
			boolArray.push_back(byte & (1 << i));
		}
	}

	//Create tmp folder
	fs::path p(inputfile);

	fs::create_directories(std::string("./tmp_" + p.stem().string()));


	//Size of the file in bits devided by pixels of a frame
	const double imageToGenerate_d = (float)boolArray.size() / (float)(WIDTH * HEIGHT);
	int imagesToGenerate = ceil(imageToGenerate_d);

	//Debug
	std::cout << "File size: " << boolArray.size() << " bits" << "\nImages float: " << imageToGenerate_d << std::endl;


	std::cout << "Generating " << imagesToGenerate << " images..." << std::endl;
	for (int i = 0; i < imagesToGenerate; i++)
	{

		// Create an empty image with a size of 1280x720 and 8 bits per color channel
		cv::Mat image(HEIGHT, WIDTH, CV_8UC3);

		for (int y = 0; y < image.rows; y++) {
			for (int x = 0; x < image.cols; x++) {
				int currentbit = WIDTH * HEIGHT * i + y * WIDTH + x;

				if (currentbit < size(boolArray))
				{
					//True = white pixel Flase = black pixel

					if (boolArray[currentbit])
						image.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
					else
						image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
				}
				else
				{
					//Overflow is black
					image.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 150, 0);
				}
			}
		}
		std::cout << "Generated image " << i + 1 << " out of " << imagesToGenerate << std::endl;

		//Get image name generated
		std::string buf(std::string("./tmp_" + p.stem().string()) + "/frame_" + std::to_string(i) + ".jpg");

		//Upscale it 4 times
		cv::Mat upscaled;
		cv::resize(image, upscaled, cv::Size(), 4, 4, cv::INTER_NEAREST);

		//Save image
		cv::imwrite(buf.c_str(), upscaled);

		//Release memory
		image.release();
		upscaled.release();
	}


	//Use ffmpeg to put that up to a 24 fps video in 1280x720
	std::string outfilename;
	if (outputfile == NULL)
		outfilename = p.filename().string() + ".mp4";
	else
		outfilename = outputfile;

	system(("ffmpeg -start_number 0 -i " + std::string("./tmp_" + p.stem().string()) + "/frame_%d.jpg -r 24 -codec copy " + outfilename).c_str());

	//Delete tmp folder
	fs::remove_all(std::string("./tmp_" + p.stem().string()));
}


//Decoding the file
void decode(const char* inputfile, const char* outputfile)
{
	char result[1024];
	sprintf_s(result, sizeof(result), "Decoding: %s to: %s", inputfile, outputfile);
	std::cout << result << std::endl;

	//Create temporary folder
	fs::path p(inputfile);
	fs::create_directories(std::string("./tmp_" + p.stem().string()));

	//Export frames using ffmpeg
	system(std::string("ffmpeg -i \"" + std::string(inputfile) + "\" " + std::string("./tmp_" + p.stem().string()) + "/frame_%d.jpg").c_str());

	//Get file count
	auto dirIter = fs::directory_iterator(std::string("./tmp_" + p.stem().string()));
	int fileCount = std::count_if(
		begin(dirIter),
		end(dirIter),
		[](auto& entry) { return entry.is_regular_file(); }
	);
	std::vector<unsigned char> binaryData;
	for (int o = 0; o < fileCount; o++)
	{
		cv::Mat bimage = cv::imread(std::string("./tmp_" + p.stem().string() + "/frame_" + std::to_string(o + 1) + ".jpg").c_str());

		//Lower the resolution of the picture
		cv::Mat image;
		cv::resize(bimage, image, cv::Size(), 0.25, 0.25, cv::INTER_AREA);
		//Start writing file

		for (int y = 0; y < HEIGHT; y++)
		{
			for (int x = 0; x < WIDTH; x += 8)
			{
				unsigned char byte = 0;
				for (int j = 0; j < 8; j++) {
					if (image.at<cv::Vec3b>(y, x + j)[1] > 100 && (image.at<cv::Vec3b>(y, x + j)[0] + image.at<cv::Vec3b>(y, x + j)[2]) < 60)
						goto ended;
					byte |= ((image.at<cv::Vec3b>(y, x + j)[0] + image.at<cv::Vec3b>(y, x + j)[1] + image.at<cv::Vec3b>(y, x + j)[2]) > 380) << (7 - j);
				}
				binaryData.push_back(byte);
			}
		}

	}
ended:
	//Choose name
	std::string outfilename;
	if (outputfile == NULL)
		outfilename = p.filename().string() + ".bin";
	else
		outfilename = outputfile;

	//Write file
	std::ofstream outfile(outfilename, std::ios::binary);
	if (outfile) {
		if (!binaryData.empty()) {
			outfile.write(reinterpret_cast<const char*>(&binaryData[0]), binaryData.size());
			std::cout << "Binary data saved to file: " << outfilename << std::endl;
		}
		else {
			std::cout << "No binary data to save." << std::endl;
		}
	}
	else {
		std::cerr << "Error opening file: " << outfilename << std::endl;
	}

	//Delete temporary folder
	fs::remove_all(std::string("./tmp_" + p.stem().string()));
}

int main(int argc, char** argv)
{
	method = argv[1];
	inputfile = argv[2];
	outputfile = argv[3];
	if (strcmp(method, "encode") == 0) encode(inputfile, outputfile);
	else if (strcmp(method, "decode") == 0) decode(inputfile, outputfile);
	return 0;
}
