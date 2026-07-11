#include "Crypto.hpp"
#include <QCryptographicHash>
#include <QString>

namespace Optix::Utilities {

std::string Crypto::hashPassword(const std::string& password) {
    QByteArray hash = QCryptographicHash::hash(
        QByteArray::fromRawData(password.data(), static_cast<int>(password.size())), 
        QCryptographicHash::Sha256
    );
    return hash.toHex().toStdString();
}

} // namespace Optix::Utilities
