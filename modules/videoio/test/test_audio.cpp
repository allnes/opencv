#include <tuple>

namespace opencv_test { namespace {

std::string bit_1[] = 
{
    "8", "16", "24", "32"
};

std::string bit_2[] = 
{
    "16", "24" 
};

std::string channels[] = 
{
    "1", "2"
};

std::string hz[] =
{
    "44100"
};

std::string audio_format_1[] =
{
    //"wav", 
    //"mp4", 
    "mp3", 
    //"ogg"
};

std::string audio_format_2[] =
{
    "flac"
};


typedef std::tuple<std::string, std::string, std::string, std::string> Param;

//#define UPDATE_AUDIO_TEST_DATA
#ifdef  UPDATE_AUDIO_TEST_DATA

TEST (AUDIO_generate,  generate_test_data)
{
    const std::string dataset_config = findDataFile("audio/dataset_config.json");
    FileStorage file_config(dataset_config, FileStorage::WRITE);
    file_config << "test_audio" << "[";
    
    for(const std::string& item4: audio_format)
    {
        for (const std::string& item1: bit)
        {
            for(const std::string& item2: channels)
            {
                for(const std::string& item3: hz)
                {
                        file_config << "{:" << "audio_name" << "test_" + item1 + "bit_" + item2 + "channels_" + item3 + "hz." + item4;
                        file_config << "audio_data_name" << "test_" + item1 + "bit_" + item2 + "channels_" + item3 + "hz.bin";
                        file_config << "}";
                }
                
            }
        }
    }
    file_config << "]";
    file_config.release();
}

#else

class AudioTestFixture : public testing::TestWithParam <Param>
{

protected:
    const std::string bit;
    const std::string channels;
    const std::string hz;
    const std::string audio_format;

    const std::string root;
    const std::string audio_name;

    std::string str;
    std::vector<unsigned char> bin;

    Mat test_data_known;
    Mat test_data_received;
public:    
    AudioTestFixture() : root("audio/"+ audio_format + "/"), bit(get<0>(GetParam())), channels(get<1>(GetParam())), hz(get<2>(GetParam())), audio_format(get<3>(GetParam())), 
    audio_name("test_" + bit + "bit_" + channels + "channels_" + hz + "hz." + audio_format), str(""), test_data_received(Mat()), test_data_known(Mat())
    {
        get_test_data_from_file();
    };
private:
    void get_test_data_from_file()
    {
        std::ifstream file;
        file.open(findDataFile(root + static_cast<string>("test_" + bit + "bit_" + channels + "channels_" + hz + "hz_"+ audio_format + ".bin")), std::ios::binary);
        ASSERT_TRUE(file.is_open());
        char tmp;
        if (file) 
        {
            while(!file.eof()) 
            {
                file.read(&tmp, 1);
                bin.push_back(tmp);
            }
        }
        //std::cout << bin.size() << std::endl;
        file.close();
        switch(std::stoi(bit))
        {
            case 8:
                test_data_known = Mat(bin.size(), std::stoi(channels), CV_8S, bin.data());
                break;
            case 16:
                test_data_known = Mat(static_cast<int>(bin.size()/2), std::stoi(channels), CV_16S, bin.data());
                break;
            case 24:
                test_data_known = Mat(bin.size(), std::stoi(channels), CV_8S, bin.data());
                break;
            case 32:
                test_data_known = Mat(static_cast<int>(bin.size()/4), std::stoi(channels), CV_32S, bin.data());
                break;
            default:
                break;       
        }
        ASSERT_FALSE(test_data_known.empty());
    }
};

class SetAudioFormat_1 : public AudioTestFixture {};
class SetAudioFormat_2 : public AudioTestFixture {};


TEST_P(AudioTestFixture, audio) 
{
    Mat frame;
    Mat diff;
    VideoCapture cap;
    int apiID = cv::CAP_MSMF;

    cap.open(findDataFile(root + audio_name), apiID);
    //cap.set(CAP_SWITCH_AUDIO_STREAM,1);
    ASSERT_TRUE(cap.isOpened());
    int count = 0;
    for (;;)
    {
        cap.read(frame);
        test_data_received.push_back(frame);
        if (frame.empty()) 
        {
            break;
        }
        count++;
    }
    ASSERT_FALSE(test_data_received.empty());
    //cv::compare(test_data_received, test_data_known, diff, cv::CMP_EQ);

    //std::cout << "test_data_received " << test_data_received.rows << std::endl;
    //std::cout << "test_data_known " << test_data_known.rows << std::endl;
    //std::cout << "BIN  " << bin.size() << std::endl;
    //std::cout << "count " << count << std::endl;

    for(int i = 0; i < test_data_known.rows; i++)
    {
        for(int j = 0; j < test_data_known.cols; j++)
        {
            //std::cout << (int)test_data_known.at<short int>(i,j) << " ! " << 
            //(int)test_data_received.at<short int>(i,j) << std::endl;
            //ASSERT_TRUE(static_cast<unsigned char>((diff.at<signed char>(i,j)))==255);
        }  
    }
}


INSTANTIATE_TEST_CASE_P(/**/, AudioTestFixture, testing::Combine(testing::ValuesIn(bit_1), testing::ValuesIn(channels), testing::ValuesIn(hz), testing::ValuesIn(audio_format_1)));
//INSTANTIATE_TEST_CASE_P(/**/, AudioTestFixture, testing::Combine(testing::ValuesIn(bit_2), testing::ValuesIn(channels), testing::ValuesIn(hz), testing::ValuesIn(audio_format_2)));

#endif // UPDATE_QRCODE_TEST_DATA
}} //namespace