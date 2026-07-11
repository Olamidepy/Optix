#pragma once
#include <string>
#include <vector>

namespace cv {
class Mat;
}

namespace Optix::FaceRecognition {

class IFaceRecognizer {
public:
    virtual ~IFaceRecognizer() = default;

    // Train the recognizer with a set of student IDs and lists of face matrices
    virtual bool train(
        const std::vector<std::string>& student_ids, 
        const std::vector<std::vector<cv::Mat>>& face_images
    ) = 0;

    // Predict student_id from a face image. Returns empty string if not recognized.
    virtual std::string predict(const cv::Mat& face_image, double& out_confidence) = 0;

    // Save trained model variables to file (e.g. models/lbph_model.yml)
    virtual bool save(const std::string& path) const = 0;

    // Load trained model variables from file
    virtual bool load(const std::string& path) = 0;
};

} // namespace Optix::FaceRecognition
