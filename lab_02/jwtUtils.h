#pragma once
#include <Poco/JWT/Token.h>
#include <Poco/JWT/Signer.h>
#include <Poco/SHA2Engine.h>
#include <Poco/DigestEngine.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/JSON/Object.h>
#include <iostream>

using namespace std;
using namespace Poco::Net;

inline const string JWT_SECRET = "MyTotallySecure256BitSecret";

struct AuthResult {
    bool ok = false;
    int userId = 0;
    string login;
};

inline string hashPassword(const string& password) {
    Poco::SHA2Engine engine(Poco::SHA2Engine::SHA_256);
    engine.update(password);
    return Poco::DigestEngine::digestToHex(engine.digest());
}

inline string generateJwt(int userId, const string& login) {
    Poco::JWT::Token token;
    token.setType("JWT");
    token.setSubject(to_string(userId));
    token.payload().set("login", login);
    token.setIssuedAt(Poco::Timestamp());
    token.setExpiration(Poco::Timestamp() + 3600);

    Poco::JWT::Signer signer(JWT_SECRET);
    return signer.sign(token, Poco::JWT::Signer::ALGO_HS256);
}

inline bool verifyJwt(const string& jwt, int& outUserId, string& outLogin) {
    try {
        Poco::JWT::Signer signer(JWT_SECRET);
        auto token = signer.verify(jwt);
        outUserId = stoi(token.getSubject());
        outLogin = token.payload().getValue<string>("login");
        return true;
    }
    catch (const exception& e) {
        cerr << "JWT verify failed: " << e.what() << endl;
        return false;
    }
}

inline AuthResult checkAuth(HTTPServerRequest& request, HTTPServerResponse& response) {
    AuthResult result;

    string header = request.get("Authorization", "");
    if (header.size() < 8 || header.substr(0, 7) != "Bearer ") {
        response.setStatus(HTTPResponse::HTTP_UNAUTHORIZED);
        response.setContentType("application/json");
        Poco::JSON::Object error;
        error.set("error", "Missing token");
        error.stringify(response.send());
        return result;
    }

    string token = header.substr(7);
    if (!verifyJwt(token, result.userId, result.login)) {
        response.setStatus(HTTPResponse::HTTP_UNAUTHORIZED);
        response.setContentType("application/json");
        Poco::JSON::Object error;
        error.set("error", "Invalid token");
        error.stringify(response.send());
        return result;
    }

    result.ok = true;
    return result;
}