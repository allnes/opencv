// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

// Copyright (C) 2018, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.

#include "../precomp.hpp"
#include "layers_common.hpp"

namespace cv { namespace dnn {

class AXPYLayerImpl CV_FINAL : public AXPYLayer
{
public:
    AXPYLayerImpl(const LayerParams& params)
    {
        setParamsFrom(params);
    }

    // AXPY -> F = AX + Y
    // input[0]   -> A > NC
    // input[1]   -> X > NCHW
    // input[2]   -> Y > NCHW
    // outputs[0] -> F > NCHW

    virtual bool getMemoryShapes(const std::vector<MatShape> &inputs,
                                 const int requiredOutputs,
                                 std::vector<MatShape> &outputs,
                                 std::vector<MatShape> &internals) const CV_OVERRIDE
    {
        CV_Assert(inputs.size() == 3);
        outputs.push_back(inputs[1]);
        return false;
    }

    void forward(InputArrayOfArrays inputs_arr, OutputArrayOfArrays outputs_arr, OutputArrayOfArrays internals_arr) CV_OVERRIDE
    {
        CV_TRACE_FUNCTION();
        CV_TRACE_ARG_VALUE(name, "name", name.c_str());
        std::vector<Mat> inputs, outputs;
        inputs_arr.getMatVector(inputs);
        outputs_arr.getMatVector(outputs);

        Mat alpha, x_array, y_array, result;
        transpose(inputs[0] .reshape(1, inputs[0] .size[1]), alpha);
        transpose(inputs[1] .reshape(1, inputs[1] .size[1]), x_array);
        transpose(inputs[2] .reshape(1, inputs[2] .size[1]), y_array);
        transpose(outputs[0].reshape(1, outputs[0].size[1]), result);

        // std::cout << alpha.size << '\n';
        // std::cout << x_array.size << '\n';
        // std::cout << y_array.size << '\n';
        // std::cout << result.size << '\n';

        for (size_t i = 0; i < x_array.size[1]; i++)
        {
            cv::multiply(x_array.col(i), alpha.at<float>(0, i), result.col(i));
        }
        cv::add(result, y_array, result);

        // outputs.clear();
        // outputs.push_back(inputs[2].clone());
        // std::cout << outputs[0].size << '\n';
        // std::cout << outputs.size() << '\n';
    }
};

Ptr<Layer> AXPYLayer::create(const LayerParams& params)
{
    return Ptr<Layer>(new AXPYLayerImpl(params));
}

}}  // namespace cv::dnn
