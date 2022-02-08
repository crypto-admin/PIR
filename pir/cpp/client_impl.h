#include "server.h"
#include "client.h"
#include "pir/cpp/test_base.h"


namespace pir {


using namespace std;


class PIRClientImpl {
 public:

  PIRClientImpl (int db_size) {
    db_size_ = db_size;
  }
  void SetUp() { 
    
    SetUpDB(db_size_); 
  }

  void SetUpDB(size_t dbsize, size_t dimensions = 1, size_t elem_size = 128,
               bool use_ciphertext_multiplication = true) {
    db_size_ = dbsize;
    encryption_params_ = GenerateEncryptionParams(POLY_MODULUS_DEGREE, 20);
    pir_params_ =
        *(CreatePIRParameters(dbsize, elem_size, dimensions, encryption_params_,
                              use_ciphertext_multiplication));

    seal::SEALContext::Create(encryption_params_);
    client_ = *(PIRClient::Create(pir_params_));
    
  }

  // const auto& Context() { return client_->context_; }
  // std::shared_ptr<seal::Decryptor> Decryptor() { return client_->decryptor_; }
  // std::shared_ptr<seal::Encryptor> Encryptor() { return client_->encryptor_; }

  size_t db_size_;
  std::shared_ptr<PIRParameters> pir_params_;
  EncryptionParameters encryption_params_;
  std::unique_ptr<PIRClient> client_;

};


class PIRClient1:
      public PIRTestingBase {
 public:
  void SetUp() {
    const auto use_ciphertext_multiplication = true;
    const auto poly_modulus_degree = 4096;
    const auto plain_mod_bits = 20;
    const auto elem_size = 7680;
    const auto bits_per_coeff = 0;
    const auto dbsize = 10;
    const auto d = 1;
    

    SetUpParams(dbsize, elem_size, d, poly_modulus_degree, plain_mod_bits,
                bits_per_coeff, use_ciphertext_multiplication);
    GenerateDB();

    client_ = *(PIRClient::Create(pir_params_));
    server_ = *(PIRServer::Create(pir_db_, pir_params_));
  }

  unique_ptr<PIRClient> client_;
  unique_ptr<PIRServer> server_;
};

}