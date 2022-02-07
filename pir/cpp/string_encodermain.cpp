//
// Copyright 2020 the authors listed in CONTRIBUTORS.md
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "pir/cpp/string_encoder.h"

#include <iostream>
#include <memory>

#include "pir/cpp/parameters.h"
#include "pir/cpp/status_asserts.h"


using std::cout;
using std::endl;
using std::make_unique;
using std::unique_ptr;

using namespace seal;
using namespace std;
using namespace pir;

constexpr size_t POLY_MODULUS_DEGREE = 4096;

class StringEncoderTest {
 public:
 
  StringEncoderTest() {
    auto params = GenerateEncryptionParams(POLY_MODULUS_DEGREE);
    seal_context_ = seal::SEALContext::Create(params);
    if (!seal_context_->parameters_set()) {
      std::cout << "seal context init error" << std::endl;
    }
    keygen_ = make_unique<KeyGenerator>(seal_context_);
    encryptor_ = make_unique<Encryptor>(seal_context_, keygen_->public_key());
    evaluator_ = make_unique<Evaluator>(seal_context_);
    decryptor_ = make_unique<Decryptor>(seal_context_, keygen_->secret_key());
    encoder_ = std::make_unique<StringEncoder>(seal_context_);
  }

  shared_ptr<SEALContext> seal_context_;
  unique_ptr<StringEncoder> encoder_;
  unique_ptr<KeyGenerator> keygen_;
  unique_ptr<Encryptor> encryptor_;
  unique_ptr<Evaluator> evaluator_;
  unique_ptr<Decryptor> decryptor_;
};

int main() {
  StringEncoderTest impl;

  string value("This is a string test for random VALUES@!#");
  size_t num_coeff = ceil((value.size() * 8) / 19.0);
  Plaintext pt;
  auto res = impl.encoder_->encode(value, pt);
  if (pt.coeff_count() ==  num_coeff) std::cout << "coeff_cout ok " << std::endl;
  auto result =  impl.encoder_->decode(pt);
  if ((*result).size() == value.size()) std::cout << "result size == value size" << std::endl;
  // EXPECT_EQ(result.substr(0, value.size()), value);
  std::cout << "substr(0, value.size) = " << (*result).substr(0, value.size()) << std::endl;
  std::cout << "substr(value.size) = " << (*result).substr(value.size()) << std::endl;
  // EXPECT_THAT(result.substr(value.size()), Each(0));

  // PNG test
  auto prng =
      seal::UniformRandomGeneratorFactory::DefaultFactory()->create({42});
  string v(28, 0);
  for (int i = 0; i < 28; i++) v[i] = 'x';
  std::cout << "v = " << std::endl;
  // prng->generate(v.size(), reinterpret_cast<SEAL_BYTE *>(v.data()));
  // Plaintext pt;
  auto res1 = impl.encoder_->encode(v, pt);
  auto result1 =  impl.encoder_->decode(pt);
  std::cout << "size = " << v.size() << std::endl;
  std::cout << "substr(0, value.size) = " << (*result1).substr(0, v.size()) << std::endl;
  // EXPECT_EQ(result.substr(0, v.size()), v);
  //EXPECT_THAT(result.substr(v.size()), Each(0));
  return 0;
}
