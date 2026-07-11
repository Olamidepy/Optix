#pragma once
#include "Repositories/IStudentRepository.hpp"
#include "FaceRecognition/IFaceRecognizer.hpp"
#include <memory>
#include <string>
#include <vector>

namespace cv {
class Mat;
}

namespace Optix::Services {

class FaceService {
public:
    FaceService(
        std::shared_ptr<Repositories::IStudentRepository> studentRepo,
        std::shared_ptr<FaceRecognition::IFaceRecognizer> recognizer
    );
    ~FaceService() = default;

    // Checks if the face recognition model file (models/lbph_model.yml) exists
    bool isModelTrained() const;

    // Triggers training of the recognizer using raw images inside datasets/raw/
    // Returns status, progress is reported via callback or out variables
    bool trainModel(int& outTotalTrained);

    // Save a raw face sample matrix to datasets/raw/{student_id}/sample_{index}.jpg
    bool saveFaceSample(const std::string& student_id, int sample_index, const cv::Mat& image);

    // Recognize student from a captured face frame. Returns Student ID if recognized above threshold.
    std::string recognizeStudent(const cv::Mat& face_image, double& out_confidence);

    // Utility to ensure dataset folders exist
    bool createStudentDatasetDirectory(const std::string& student_id);

    // Get model file path
    std::string getModelPath() const { return m_modelPath; }

private:
    std::shared_ptr<Repositories::IStudentRepository> m_studentRepo;
    std::shared_ptr<FaceRecognition::IFaceRecognizer> m_recognizer;

    std::string m_datasetRootDir{"datasets/raw"};
    std::string m_modelPath{"models/lbph_model.yml"};
};

} // namespace Optix::Services
