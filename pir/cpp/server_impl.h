
#include <algorithm>
#include <iostream>
#include <vector>

#include "pir/cpp/server.h"
#include "pir/cpp/client.h"
#include "pir/cpp/ct_reencoder.h"
#include "pir/cpp/status_asserts.h"
#include "pir/cpp/test_base.h"
#include "pir/cpp/utils.h"

namespace pir {
namespace {  // local const;
using std::cout;
using std::endl;
using std::get;
using std::make_tuple;
using std::make_unique;
using std::shared_ptr;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;

using seal::Ciphertext;
using seal::GaloisKeys;
using seal::Plaintext;
using seal::RelinKeys;

using namespace seal;
using std::int64_t;
using std::vector;

constexpr uint32_t POLY_MODULUS_DEGREE = 4096;
// constexpr uint32_t ELEM_SIZE = 128;

// struct pirparams {
//   uint32_t poly_modulus_degree;
//   uint32_t ele_size;
//   uint8_t dimensions;
//   uint32_t plain_mod_bit_size;
//   bool use_ciphertext_multiplication;
//   uint64_t db_size;
// };
 
pirparams testparam = {4096, 128, 1, 20, true, 10}; 

class PIRServerImpl : public PIRTestingBase {
 public:
  PIRServerImpl (pirparams param = testparam) {
    // db_size_ = db_size;
    param_  = param;
  }

  void SetUp() {   
    SetUpDBImpl(param_.db_size, 
                param_.ele_size, 
                param_.dimensions,  
                param_.plain_mod_bit_size, 
                param_.use_ciphertext_multiplication); 
   }

  void SetUpDBImpl(size_t dbsize, size_t elem_size, size_t dimensions = 1, 
                   uint32_t plain_mod_bit_size = 20,
                   bool use_ciphertext_multiplication = true) {
                     std::cout << "server db dimension = " << dimensions << std::endl;
    SetUpParams(dbsize, elem_size, dimensions, POLY_MODULUS_DEGREE,
                plain_mod_bit_size, 0, use_ciphertext_multiplication);
    // GenerateIntDB();
    GenerateDB();
    SetUpSealTools();

    // gal_keys_ =
         // keygen_->galois_keys_local(generate_galois_elts(POLY_MODULUS_DEGREE));
    // relin_keys_ = keygen_->relin_keys_local();

    server_ = *(PIRServer::Create(pir_db_, pir_params_));
    client_ = *(PIRClient::Create(pir_params_));  // client is for test;
    // TODO: add assert here.
  }

  unique_ptr<PIRServer> server_;
  unique_ptr<PIRClient> client_;  // just for test;
  // int db_size_;
  pirparams param_;
  // GaloisKeys gal_keys_;
  // RelinKeys relin_keys_;
};

}
}