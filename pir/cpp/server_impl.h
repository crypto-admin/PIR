
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
namespace {

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
constexpr uint32_t ELEM_SIZE = 7680;

class PIRServerInit : public PIRTestingBase {
  public:
  void SetUpDBImpl(size_t dbsize, size_t dimensions = 1,
                   size_t elem_size = ELEM_SIZE,
                   uint32_t plain_mod_bit_size = 20,
                   bool use_ciphertext_multiplication = false) {
    SetUpParams(dbsize, elem_size, dimensions, POLY_MODULUS_DEGREE,
                plain_mod_bit_size, 0, use_ciphertext_multiplication);
    GenerateIntDB();
    SetUpSealTools();

    gal_keys_ =
        keygen_->galois_keys_local(generate_galois_elts(POLY_MODULUS_DEGREE));
    relin_keys_ = keygen_->relin_keys_local();

    server_ = *(PIRServer::Create(pir_db_, pir_params_));
    client_ = *(PIRClient::Create(pir_params_));
    // TODO: add assert here.
  }

  unique_ptr<PIRServer> server_;
  unique_ptr<PIRClient> client_;
  GaloisKeys gal_keys_;
  RelinKeys relin_keys_;
};

class PIRServerImpl : public PIRServerInit {
 public:
   PIRServerImpl (int db_size) {
    db_size_ = db_size;
  }
  
  void SetUp() { SetUpDB(db_size_); }
  void SetUpDB(size_t dbsize, size_t dimensions = 1,
               size_t elem_size = ELEM_SIZE, uint32_t plain_mod_bit_size = 20) {
    SetUpDBImpl(dbsize, dimensions, elem_size, plain_mod_bit_size, true); // TODO :use ciphertext multiplication
  }
 
  int db_size_;
};

}
}