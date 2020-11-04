#include <tuple>

namespace opencv_test { namespace {

int bit_per_sample_1[] = 
{
    8, 16, 24, 32
};

int bit_per_sample_2[] = 
{
    16, 24
};

int number_channels[] = 
{
    1, 2
};

int sampling_frequency[] =
{
    44100
};

std::string audio_format_1[] =
{
    "wav", "mp4", "mp3", "aac", "m4a", "wma", "flac"//"ogg"
};

std::string audio_format_2[] =
{
    "flac"
};

std::pair<std::string, int> backend[] =
{
    {"CAP_MSMF", cv::CAP_MSMF}
};

typedef std::tuple<int, int, int, std::string, std::pair<std::string, int> > Param;

TEST(Audio, generate_test_data)
{
    
}

class Environment : public testing::Environment {
 public:
  // Override this to define how to set up the environment.
  void SetUp() override {}
};

class AudioTestFixture : public testing::TestWithParam <Param>
{
protected:
    const int bit_per_sample;
    const int number_channels;
    const int sampling_frequency;
    const std::string audio_format;
    const std::pair<std::string, int> backend;
    int bit_per_channel;
    const std::string root;
    const std::string audio_name;
    std::vector<unsigned char> bin;

    Mat test_data_known;
    Mat test_data_received;
public:    
    AudioTestFixture() : 
    bit_per_sample(get<0>(GetParam())), 
    number_channels(get<1>(GetParam())), 
    sampling_frequency(get<2>(GetParam())), 
    audio_format(get<3>(GetParam())), 
    backend(get<4>(GetParam())),
    root("audio/"+ audio_format + "/"),
    audio_name("test_" + std::to_string(bit_per_sample) + "bit_" + std::to_string(number_channels) + "channels_" + std::to_string(sampling_frequency) + "hz.") 
    { 
        get_test_data_from_bin_file();
        if(bin.empty())
            std::cout << "Audio codec don't support this param \n";
        bit_per_channel = (int)bin.size()/number_channels;
        switch(bit_per_sample)
        {
            case 8:
                test_data_known = Mat(bit_per_channel, number_channels, CV_8S, bin.data());
                break;
            case 16:
                test_data_known = Mat(bit_per_channel/2, number_channels, CV_16S, bin.data());
                break;
            case 24:
                test_data_known = Mat(bit_per_channel, number_channels, CV_8U, bin.data());
                break;
            case 32:
                test_data_known = Mat(bit_per_channel/4, number_channels, CV_32S, bin.data());
                break;
            default:
                break;       
        } 
    };
private:
    void get_test_data_from_bin_file()
    {
        std::ifstream file;
        if(audio_format == "wav" || audio_format == "flac")
        {
            file.open(findDataFile(root + audio_name + "bin"), std::ios::binary);
        }
        else
        {
            file.open(findDataFile(root + backend.first + "/" + audio_name + "bin"), std::ios::binary);
        }
        ASSERT_TRUE(file.is_open());
        char tmp;
        while (file.read(&tmp, sizeof(tmp)))
        {
            bin.push_back(tmp);
        }
        file.close();
    }
};

TEST_P(AudioTestFixture, audio) 
{
    Mat frame;
    Mat diff;
    VideoCapture cap;
    int apiID = backend.second;
    if(!(bin.empty()))
    {
        ASSERT_FALSE(test_data_known.empty());
        ASSERT_TRUE(cap.open(findDataFile(root + audio_name + audio_format), apiID));
        ASSERT_TRUE(cap.set(CAP_SWITCH_AUDIO_STREAM,1));
        ASSERT_TRUE(cap.set(CAP_PROP_BPS, bit_per_sample));
        for (;;)
        {
            cap.read(frame);
            test_data_received.push_back(frame);
            if (frame.empty()) 
            {
                break;
            }
        }
        ASSERT_FALSE(test_data_received.empty());
        cv::compare(test_data_received, test_data_known, diff, cv::CMP_EQ);
        for(int i = 0; i < test_data_known.rows; i++)
        {
            for(int j = 0; j < test_data_known.cols; j++)
            {
                ASSERT_TRUE(diff.at<unsigned char>(i,j)==255);
            }  
        }
    }
}

INSTANTIATE_TEST_CASE_P(/**/, AudioTestFixture, testing::Combine(testing::ValuesIn(bit_per_sample_1), testing::ValuesIn(number_channels), testing::ValuesIn(sampling_frequency), testing::ValuesIn(audio_format_1), testing::ValuesIn(backend)));
//INSTANTIATE_TEST_CASE_P(/**/, AudioTestFixture, testing::Combine(testing::ValuesIn(bit_per_sample_2), testing::ValuesIn(number_channels), testing::ValuesIn(sampling_frequency), testing::ValuesIn(audio_format_2), testing::ValuesIn(backend)));

}} //namespace