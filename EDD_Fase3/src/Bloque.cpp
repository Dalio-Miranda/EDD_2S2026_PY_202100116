#include "Bloque.h"
#include "SHA256.h"
#include <sstream>
#include <ctime>

std::string Bloque::calcularHash() const {
    std::ostringstream oss;
    oss << index << timestamp << nonce << rootMerkle << previousHash;
    return SHA256::calcular(oss.str());
}

void Bloque::minar() {
    std::string prefijoValido(DIFICULTAD_PRUEBA_TRABAJO, '0');

    nonce = 0;
    while (true) {
        std::string candidato = calcularHash();
        if (candidato.compare(0, DIFICULTAD_PRUEBA_TRABAJO, prefijoValido) == 0) {
            hash = candidato;
            return;
        }
        nonce++;
    }
}

bool Bloque::esValido() const {
    return hash == calcularHash();
}

std::string Bloque::timestampActual() {
    std::time_t ahora = std::time(nullptr);
    std::tm* t = std::localtime(&ahora);
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%02d-%02d-%04d::%02d:%02d:%02d",
                  t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
                  t->tm_hour, t->tm_min, t->tm_sec);
    return std::string(buffer);
}

void to_json(nlohmann::json& j, const Bloque& b) {
    nlohmann::json arregloData = nlohmann::json::array();
    for (int i = 0; i < b.data.getTamanio(); i++) {
        arregloData.push_back(b.data[i]);
    }

    j = nlohmann::json{
        {"INDEX", b.index},
        {"TIMESTAMP", b.timestamp},
        {"NONCE", b.nonce},
        {"DATA", arregloData},
        {"ROOTMERKLE", b.rootMerkle},
        {"PREVIOUSHASH", b.previousHash},
        {"HASH", b.hash}
    };
}

void from_json(const nlohmann::json& j, Bloque& b) {
    j.at("INDEX").get_to(b.index);
    j.at("TIMESTAMP").get_to(b.timestamp);
    j.at("NONCE").get_to(b.nonce);
    j.at("ROOTMERKLE").get_to(b.rootMerkle);
    j.at("PREVIOUSHASH").get_to(b.previousHash);
    j.at("HASH").get_to(b.hash);

    b.data.vaciar();
    for (const auto& item : j.at("DATA")) {
        b.data.agregarFinal(item.get<Transaccion>());
    }
}
