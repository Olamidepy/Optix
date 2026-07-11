#include "LBPHFaceRecognizer.hpp"
#include <opencv2/face.hpp>
#include <opencv2/core.hpp>
#include <iostream>

namespace Optix::FaceRecognition {

LBPHFaceRecognizer::LBPHFaceRecognizer() {
    // Instantiate OpenCV LBPH Face Recognizer
    m_recognizer = cv::face::LBPHFaceRecognizer::create();
}

bool LBPHFaceRecognizer::train(
    const std::vector<std::string>& student_ids, 
    const std::vector<std::vector<cv::Mat>>& face_images
) {
    if (m_recognizer.empty()) return false;
    if (student_ids.size() != face_images.size()) return false;

    m_labelToId.clear();
    m_idToLabel.clear();

    std::vector<cv::Mat> flatImages;
    std::vector<int> flatLabels;

    int currentLabel = 0;
    for (size_t i = 0; i < student_ids.size(); ++i) {
        const std::string& id = student_ids[i];
        m_labelToId[currentLabel] = id;
        m_idToLabel[id] = currentLabel;

        for (const auto& faceImg : face_images[i]) {
            if (!faceImg.empty()) {
                flatImages.push_back(faceImg);
                flatLabels.push_back(currentLabel);
            }
        }
        currentLabel++;
    }

    if (flatImages.empty()) {
        return false;
    }

    try {
        m_recognizer->train(flatImages, flatLabels);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Optix FaceRecognizer: Training error: " << e.what() << std::endl;
        return false;
    }
}

std::string LBPHFaceRecognizer::predict(const cv::Mat& face_image, double& out_confidence) {
    if (m_recognizer.empty() || m_labelToId.empty() || face_image.empty()) {
        out_confidence = 0.0;
        return "";
    }

    try {
        int label = -1;
        double confidence = 0.0;
        m_recognizer->predict(face_image, label, confidence);
        
        out_confidence = confidence;
        
        auto it = m_labelToId.find(label);
        if (label != -1 && it != m_labelToId.end()) {
            return it->second;
        }
    } catch (const std::exception& e) {
        std::cerr << "Optix FaceRecognizer: Prediction error: " << e.what() << std::endl;
    }

    return "";
}

bool LBPHFaceRecognizer::save(const std::string& path) const {
    if (m_recognizer.empty()) return false;

    try {
        m_recognizer->write(path);
        return saveMetadata(path);
    } catch (const std::exception& e) {
        std::cerr << "Optix FaceRecognizer: Save failed: " << e.what() << std::endl;
        return false;
    }
}

bool LBPHFaceRecognizer::load(const std::string& path) {
    if (m_recognizer.empty()) return false;

    try {
        m_recognizer->read(path);
        return loadMetadata(path);
    } catch (const std::exception& e) {
        std::cerr << "Optix FaceRecognizer: Load failed: " << e.what() << std::endl;
        return false;
    }
}

bool LBPHFaceRecognizer::saveMetadata(const std::string& path) const {
    try {
        cv::FileStorage fs(path, cv::FileStorage::APPEND);
        if (!fs.isOpened()) return false;

        fs << "optix_label_mapping" << "[";
        for (const auto& [label, id] : m_labelToId) {
            fs << "{:" << "label" << label << "student_id" << id << "}";
        }
        fs << "]";
        fs.release();
        return true;
    } catch (...) {
        return false;
    }
}

bool LBPHFaceRecognizer::loadMetadata(const std::string& path) {
    try {
        cv::FileStorage fs(path, cv::FileStorage::READ);
        if (!fs.isOpened()) return false;

        m_labelToId.clear();
        m_idToLabel.clear();

        cv::FileNode mapping = fs["optix_label_mapping"];
        if (mapping.type() == cv::FileNode::SEQ) {
            for (auto it = mapping.begin(); it != mapping.end(); ++it) {
                int label = static_cast<int>((*it)["label"]);
                std::string id = static_cast<std::string>((*it)["student_id"]);
                m_labelToId[label] = id;
                m_idToLabel[id] = label;
            }
        }
        fs.release();
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace Optix::FaceRecognition
