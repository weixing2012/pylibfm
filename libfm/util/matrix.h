//
// Created by yanyu on 2017/7/4.
// 定义矩阵和向量
//

#ifndef PYLIBFM_MATRIX_H
#define PYLIBFM_MATRIX_H

#include <vector>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "../util/random.h"

//版本号
const uint DVECTOR_EXPECTED_FILE_ID = 1;
const uint DMATRIX_EXPECTED_FILE_ID = 1001;

//文件头
struct DMatrixFileHeader {
    uint id;
    uint type_size;
    uint num_rows;
    uint num_cols;
};

//稠密矩阵Dense Matrix
template <typename T> class DMatrix {
public:
    T* value;

    uint dim1, dim2;
    uint max_size = {0};

    T get(uint x, uint y) {
        return value[x * dim2 + y];
    }

    DMatrix(uint p_dim1, uint p_dim2) {
        dim1 = 0;
        dim2 = 0;
        value = NULL;
        setSize(p_dim1, p_dim2);
    }

    DMatrix() {
        dim1 = 0;
        dim2 = 0;
        value = NULL;
    }

    ~DMatrix() {
        if (value != NULL) {
            delete [] value;
        }
    }

    void assign(DMatrix<T>& v) {
        if ((v.dim1 != dim1) || (v.dim2 != dim2)) { setSize(v.dim1, v.dim2); }
        for (uint i = 0; i < dim1; i++) {
            for (uint j = 0; j < dim2; j++) {
                value[i * dim2 + j] = v.value[i * dim2 + j];
            }
        }
    }
    void init(T v) {
        for (uint i = 0; i < dim1; i++) {
            for (uint i2 = 0; i2 < dim2; i2++) {
                value[i * dim2 + i2] = v;
            }
        }
    }
    void setSize(uint p_dim1, uint p_dim2) {
        if(p_dim1 * p_dim2 > max_size){
            if (value != NULL) {
                delete [] value;
            }
            max_size = p_dim1 * p_dim2;
            value = new T[max_size];
        }

        dim1 = p_dim1;
        dim2 = p_dim2;
    }

    T& operator() (unsigned x, unsigned y) {
        //	assert((x < dim1) && (y < dim2));
        return value[x * dim2 + y];
    }
    T operator() (unsigned x, unsigned y) const {
        //	assert((x < dim1) && (y < dim2));
        return value[x * dim2 + y];
    }

    T* operator() (unsigned x) const {
        //	assert((x < dim1));
        return value[x];
    }



    void saveToBinaryFile(std::string filename) {
        std::cout << "writing to " << filename << std::endl; std::cout.flush();
        std::ofstream out(filename.c_str(), std::ios_base::out | std::ios_base::binary);
        if (out.is_open()) {
            DMatrixFileHeader fh;
            fh.id = DMATRIX_EXPECTED_FILE_ID;
            fh.num_rows = dim1;
            fh.num_cols = dim2;
            fh.type_size = sizeof(T);
            out.write(reinterpret_cast<char*>(&fh), sizeof(fh));
            for (uint i = 0; i < dim1; i++) {
                out.write(reinterpret_cast<char*>(value[i]), sizeof(T)*dim2);
            }
            out.close();
        } else {
            throw "could not open " + filename;
        }
    }

    void loadFromBinaryFile(std::string filename) {
        std::cout << "reading " << filename << std::endl; std::cout.flush();
        std::ifstream in(filename.c_str(), std::ios_base::in | std::ios_base::binary);
        if (in.is_open()) {
            DMatrixFileHeader fh;
            in.read(reinterpret_cast<char*>(&fh), sizeof(fh));
            assert(fh.id == DMATRIX_EXPECTED_FILE_ID);
            assert(fh.type_size == sizeof(T));
            setSize(fh.num_rows, fh.num_cols);
            for (uint i = 0; i < dim1; i++) {
                in.read(reinterpret_cast<char*>(value[i]), sizeof(T)*dim2);
            }
            in.close();
        } else {
            throw "could not open " + filename;
        }
    }

};

template <typename T> class DVector {
public:
    uint dim;
    uint max_dim;
    T* value;
    DVector() {
        dim = 0;
        max_dim = 0;
        value = NULL;
    }
    DVector(uint p_dim) {
        dim = 0;
        max_dim = 0;
        value = NULL;
        setSize(p_dim);
    }
    ~DVector() {
        if (value != NULL) {
            delete [] value;
        }
    }
    T get(uint x) {
        return value[x];
    }
    void setSize(uint p_dim) {
        if (p_dim > max_dim) {
            if (value != NULL) {
                delete [] value;
            }
            value = new T[p_dim];
            max_dim = p_dim;
        }
        dim = p_dim;
    }
    T& operator() (unsigned x) {
        return value[x];
    }
    T operator() (unsigned x) const {
        return value[x];
    }
    void init(T v) {
        for (uint i = 0; i < dim; i++) {
            value[i] = v;
        }
    }
    void assign(T* v) {
        if (v->dim != dim) { setSize(v->dim); }
        for (uint i = 0; i < dim; i++) {
            value[i] = v[i];
        }
    }
    void assign(DVector<T>& v) {
        if (v.dim != dim) { setSize(v.dim); }
        for (uint i = 0; i < dim; i++) {
            value[i] = v.value[i];
        }
    }


    void saveToBinaryFile(std::string filename) {
        std::ofstream out (filename.c_str(), std::ios_base::out | std::ios_base::binary);
        if (out.is_open())	{
            uint file_version = DVECTOR_EXPECTED_FILE_ID;
            uint data_size = sizeof(T);
            uint num_rows = dim;
            out.write(reinterpret_cast<char*>(&file_version), sizeof(file_version));
            out.write(reinterpret_cast<char*>(&data_size), sizeof(data_size));
            out.write(reinterpret_cast<char*>(&num_rows), sizeof(num_rows));
            out.write(reinterpret_cast<char*>(value), sizeof(T)*dim);
            out.close();
        } else {
            std::cout << "Unable to open file " << filename;
        }
    }

    void loadFromBinaryFile(std::string filename) {
        std::ifstream in (filename.c_str(), std::ios_base::in | std::ios_base::binary);
        if (in.is_open())	{
            uint file_version;
            uint data_size;
            uint num_rows;
            in.read(reinterpret_cast<char*>(&file_version), sizeof(file_version));
            in.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));
            in.read(reinterpret_cast<char*>(&num_rows), sizeof(num_rows));
            assert(file_version == DVECTOR_EXPECTED_FILE_ID);
            assert(data_size == sizeof(T));
            setSize(num_rows);
            in.read(reinterpret_cast<char*>(value), sizeof(T)*dim);
            in.close();
        } else {
            std::cout << "Unable to open file " << filename;
        }
    }
};


class DVectorFloat : public DVector<float> {
public:
    void init_normal(float mean, float stdev) {
        for (uint i_2 = 0; i_2 < dim; i_2++) {
            value[i_2] = ran_gaussian(mean, stdev);
        }
    }
};

class DMatrixFloat : public DMatrix<float> {
public:
    void init(float mean, float stdev) {
        for (uint i_1 = 0; i_1 < dim1; i_1++) {
            for (uint i_2 = 0; i_2 < dim2; i_2++) {
                value[i_1* dim2 + i_2] = ran_gaussian(mean, stdev);
            }
        }
    }
    void init_column(float mean, float stdev, int column) {
        for (uint i_1 = 0; i_1 < dim1; i_1++) {
            value[i_1* dim2 + column] = ran_gaussian(mean, stdev);
        }
    }
};

#endif //PYLIBFM_MATRIX_H
