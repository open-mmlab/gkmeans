//
// Created by alex on 7/14/15.
//

#ifndef GKMEANS_CONTROLLERS_H
#define GKMEANS_CONTROLLERS_H

#include "gkmeans/common.h"
#include "gkmeans/functions.h"
#include "gkmeans/data_providers.h"

namespace gkmeans {
/**
 * @brief the base class of controllers
 */
  template<typename Dtype>
  class Controller {
  public:
    explicit Controller() {
      round_ = 0;
    };

    virtual ~Controller(){
      for (size_t i = 0; i < funcs_.size(); ++i){
//        delete funcs_[i];
      }
      for (size_t i = 0; i < mats_.size(); ++i){
//        delete mats_[i];
      }
    }

    virtual void Seed() = 0;

    virtual void Step() = 0;

    virtual void SetUp() = 0;

    virtual void PostProcess() = 0;

    void Solve(int max_round) {
      Seed();
      for (int round = 0; round < max_round; ++round){
        Step();
      }
      PostProcess();
    };

    /** internal structure accessors */
    vector<FunctionBase<Dtype>* >& funcs(){return funcs_; }
    vector<Mat<Dtype>* >& mats(){return mats_;}
    map<string, int>& name_func_indices(){return name_func_indices_;}
    map<string, int>& name_mat_indices(){return name_mat_indices_;}
    vector<string>& func_names(){return func_names_;}
    vector<string>& mat_names(){return mat_names_;}
    vector<DataProviderBase<Dtype>* >& data_providers(){ return data_providers_;}

  protected:

    void registerFuncName(const char* name){
      this->func_names_.push_back(name);
      this->name_func_indices_.insert(pair<string, int>(name, this->func_names().size() - 1));
    }

    void registerMatName(const char* name){
      this->mat_names_.push_back(name);
      this->name_mat_indices_.insert(pair<string, int>(name, this->mat_names().size() - 1));
    }

    void markMat(vector<Mat<Dtype>* >& vec, vector<int>& id_vec_){
      vec.push_back(this->mats_.back());
      id_vec_.push_back(this->mats_.size() - 1);
    }

    vector<FunctionBase<Dtype>* > funcs_;
    vector<Mat<Dtype>* >mats_;
    vector<DataProviderBase<Dtype>* > data_providers_;

    map<string, int> name_mat_indices_;
    map<string, int> name_func_indices_;
    vector<string> mat_names_;
    vector<string> func_names_;

    vector<vector<Mat<Dtype> * > > function_input_vecs_;
    vector<vector<Mat<Dtype> * > > function_output_vecs_;
    vector<vector<int> > function_input_id_vecs_;
    vector<vector<int> > function_output_id_vecs_;

    int round_;
    cudaStream_t stream_;
  };

  /**
   * @brief the controller for running kmeans on GPU
   * This controller links one NearestNeighbor and one CenterOfMass functions to implement
   * a kmeans clustering algorithm.
   */
  template<typename Dtype>
  class KMeansController : public Controller<Dtype> {
  public:

    virtual void Seed();

    virtual void Step();

    virtual void SetUp();

    virtual void PostProcess();

  protected:

    size_t batch_size_;
    size_t M_, N_, K_;
  };
}
#endif //GKMEANS_CONTROLLERS_H