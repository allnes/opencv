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

        std::cout << inputs[1].size << " | " << outputs[0].size << '\n';
        Mat alpha;
        transpose(inputs[0].reshape(1, inputs[0].size[1]), alpha);
        std::cout << alpha.size << '\n';
        outputs_arr.getMatVector(outputs);
    }
};

Ptr<Layer> AXPYLayer::create(const LayerParams& params)
{
    return Ptr<Layer>(new AXPYLayerImpl(params));
}

}}  // namespace cv::dnn
