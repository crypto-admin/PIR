#include "server.h"
#include "client.h"
#include "pir/cpp/test_base.h"


namespace pir {


using namespace std;

// struct pirparams {
//   uint32_t poly_modulus_degree;
//   uint32_t ele_size;
//   uint8_t dimensions;
//   uint32_t plain_mod_bit_size;
//   bool use_ciphertext_multiplication;
//   uint64_t db_size;
// };
 
pirparams testparam = {4096, 128, 1, 20, true, 10};

class PIRClientImpl {
 public:
  PIRClientImpl (pirparams param = testparam) {
    // db_size_ = db_size;
    param_ = param;
  }
  void SetUp() {    
    SetUpDB(param_.db_size, param_.dimensions, param_.ele_size, param_.use_ciphertext_multiplication); 
  }

  void SetUpDB(size_t dbsize, size_t dimensions = 1, size_t elem_size = 128,
               bool use_ciphertext_multiplication = true) {
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

  // size_t db_size_;
  pirparams param_;
  std::shared_ptr<PIRParameters> pir_params_;
  EncryptionParameters encryption_params_;
  std::unique_ptr<PIRClient> client_;

};

}