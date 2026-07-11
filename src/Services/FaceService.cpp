#include "FaceService.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace Optix::Services {

FaceService::FaceService(
    std::shared_ptr<Repositories::IStudentRepository> studentRepo,
    std::shared_ptr<FaceRecognition::IFaceRecognizer> recognizer
) : m_studentRepo(std::move(studentRepo)), m_recognizer(std::move(recognizer)) {
}

bool FaceService::isModelTrained() const {
    return fs::exists(m_modelPath) && fs::file_size(m_modelPath) > 0;
}

bool FaceService::trainModel(int& outTotalTrained) {
    outTotalTrained = 0;
    if (!fs::exists(m_datasetRootDir)) {
        return false;
    }

    std::vector<std::string> studentIds;
    std::vector<std::vector<cv::Mat>> faceImages;

    // Browse through directories inside datasets/raw/
    try {
        for (const auto& entry : fs::directory_iterator(m_datasetRootDir)) {
            if (entry.is_directory()) {
                std::string studentId = entry.path().filename().string();
                std::vector<cv::Mat> images;

                // Load all images for this student
                for (const auto& imgEntry : fs::directory_iterator(entry.path())) {
                    if (imgEntry.is_regular_file()) {
                        std::string ext = imgEntry.path().extension().string();
                        // Support typical image files
                        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png") {
                            cv::Mat img = cv::imread(imgEntry.path().string(), cv::IMREAD_GRAYSCALE);
                            if (!img.empty()) {
                                images.push_back(img);
                            }
                        }
                    }
                }

                if (!images.empty()) {
                    studentIds.push_back(studentId);
                    faceImages.push_back(images);
                    outTotalTrained += static_cast<int>(images.size());
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Optix FaceService: Error reading dataset directory: " << e.what() << std::endl;
        return false;
    }

    if (studentIds.empty()) {
        return false;
    }

    // Pass images to recognizer for training
    bool success = m_recognizer->train(studentIds, faceImages);
    if (success) {
        // Create parent folders for models if not exist
        fs::path modelFilePath(m_modelPath);
        if (modelFilePath.has_parent_path()) {
            fs::create_directories(modelFilePath.parent_path());
        }
        success = m_recognizer->save(m_modelPath);
    }
    
    return success;
}

bool FaceService::saveFaceSample(const std::string& student_id, int sample_index, const cv::Mat& image) {
    if (!createStudentDatasetDirectory(student_id)) {
        return false;
    }

    // Format filename: datasets/raw/{student_id}/sample_{index}.jpg
    fs::path targetPath = fs::path(m_datasetRootDir) / student_id / ("sample_" + std::to_string(sample_index) + ".jpg");
    
    // Save image using OpenCV (ensure grayscale or color based on recognizer requirements)
    cv::Mat gray;
    if (image.channels() == 3) {
        // Needs imgproc to convert, but we can assume incoming image is already processed or we convert
        // Let's check channels and write. Imgproc is in cmake.
        // If we save it directly, cv::imwrite will handle channels.
        // Let's save the exact matrix passed.
        return cv::imwrite(targetPath.string(), image);
    }
    
    return cv::imwrite(targetPath.string(), image);
}

std::string FaceService::recognizeStudent(const cv::Mat& face_image, double& out_confidence) {
    if (!isModelTrained()) {
        out_confidence = 0.0;
        return "";
    }
    
    // Load model dynamically if it hasn't been loaded in memory yet
    // recognizer should handle loading once or caching
    return m_recognizer->predict(face_image, out_confidence);
}

bool FaceService::createStudentDatasetDirectory(const std::string& student_id) {
    try {
        fs::path dirPath = fs::path(m_datasetRootDir) / student_id;
        if (!fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Optix FaceService: Cannot create student directory: " << e.what() << std::endl;
        return false;
    }
}

} // namespace Optix::Services
