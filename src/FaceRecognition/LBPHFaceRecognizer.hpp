#pragma once
#include "IFaceRecognizer.hpp"
#ifdef OPTIX_MOCK_BACKEND
#include "Core/MockBackend.hpp"
#else
#include <opencv2/core.hpp>
namespace cv::face {
class FaceRecognizer;
}
#endif
#include <map>
#include <string>
#include <vector>

namespace Optix::FaceRecognition {

class LBPHFaceRecognizer : public IFaceRecognizer {
public:
    LBPHFaceRecognizer();
    ~LBPHFaceRecognizer() override = default;

    bool train(
        const std::vector<std::string>& student_ids, 
        const std::vector<std::vector<cv::Mat>>& face_images
    ) override;

    std::string predict(const cv::Mat& face_image, double& out_confidence) override;

    bool save(const std::string& path) const override;
    bool load(const std::string& path) override;

private:
    // Helper to save mapping metadata to the same YAML file
    bool saveMetadata(const std::string& path) const;
    
    // Helper to load mapping metadata from the YAML file
    bool loadMetadata(const std::string& path);

    // OpenCV LBPH Recognizer handle
    cv::Ptr<cv::face::FaceRecognizer> m_recognizer;

    // Mapping between OpenCV integer labels and database student string IDs
    std::map<int, std::string> m_labelToId;
    std::map<std::string, int> m_idToLabel;
};

} // namespace Optix::FaceRecognition
