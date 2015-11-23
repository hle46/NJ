#include <iostream>
#include <limits>
#include <vector>

using namespace std;

struct Tree {
  int num_nodes;
  Tree *left;
  Tree *right;
  float total_length;
  float branch_length[2];
  Tree(int _num_nodes, float _length, Tree *_left, Tree *_right, float length1,
       float length2)
      : num_nodes(_num_nodes), left(_left), right(_right),
        total_length(_length) {
    branch_length[0] = length1;
    branch_length[1] = length2;
  }
};

int getMinIdx(float *mat, int n) {
  float val = numeric_limits<float>::infinity();
  int idx = -1;
  for (int i = 0; i < n; ++i) {
    if (mat[i] != 0.0f && mat[i] < val) {
      idx = i;
      val = mat[i];
    }
  }
  return idx;
}

void update(float *mat, int n, int idx1, int idx2) {
  cout << idx1 << ", " << idx2 << "\n";
  float d = mat[n * idx1 + idx2];
  for (int i = 0; i < n; ++i) {
    float val =
      (mat[n * idx1 + i] + mat[n * idx2 + i] - d) / 2;
    mat[n * idx1 + i] = mat[n * idx1 + i] != 0.0f ? val : 0.0f;
    mat[n * idx2 + i] = 0.0f;
    mat[n * i + idx1] = mat[n * i + idx1] != 0.0f ? val : 0.0f;
    mat[n * i + idx2] = 0.0f;
  }
  mat[n * idx1 + idx1] = 0.0f;
  mat[n * idx1 + idx2] = 0.0f;
  mat[n * idx2 + idx1] = 0.0f;
  mat[n * idx2 + idx2] = 0.0f;
}

void cleanupTree(Tree *tree) {
  // Reach the leaf
  if (tree->left == nullptr && tree->right == nullptr) {
    delete tree;
    return;
  }
  cleanupTree(tree->left);
  cleanupTree(tree->right);
}

void printTree(Tree *tree) {
  // Reach the leaf
  if (tree->left == nullptr && tree->right == nullptr) {
    return;
  }
  cout << "(";
  printTree(tree->left);
  cout << ": " << tree->branch_length[0] << ", ";
  printTree(tree->right);
  cout << ": " << tree->branch_length[1] << ")";
}

int main() {
  const int num_seqs = 5;
  float a[num_seqs][num_seqs]{{0.0f, 5.0f, 9.0f, 9.0f, 8.0f},
                              {5.0f, 0.0f, 10.0f, 10.0f, 9.0f},
                              {9.0f, 10.0f, 0.0f, 8.0f, 7.0f},
                              {9.0f, 10.0f, 8.0f, 0.0f, 3.0f},
                              {8.0f, 9.0f, 7.0f, 3.0f, 0.0f}};

  Tree *nodes[num_seqs];
  for (int i = 0; i < num_seqs; ++i) {
    nodes[i] = new Tree(1, 0.0f, nullptr, nullptr, 0.0f, 0.0f);
  }

  float *q = (float *)malloc(sizeof(float) * num_seqs * num_seqs);
  vector<float> s(num_seqs);
  
  Tree *root;
  for (int remain = num_seqs; remain >= 2; --remain) {
    // calculate sums over row
    for (int i = 0; i < num_seqs; ++i) {
      s[i] = 0.0f;
      for (int j = 0; j < num_seqs; ++j) {
        s[i] += a[i][j];
      }
    }
    // calculate q matrix;
    for (int i = 0; i < num_seqs; ++i) {
      for (int j = 0; j < num_seqs; ++j) {
        q[i * num_seqs + j] =
            (a[i][j] != 0.0f) ? (remain - 2) * a[i][j] - s[i] - s[j] : 0.0f;
      }
    }

    for (int i = 0; i < num_seqs; ++i) {
      for (int j = 0; j < num_seqs; ++j) {
	cout << q[i * num_seqs + j] << ",\t";
      }
      cout << "\n";
    }
    cout << "--------------------------------------\n";

    int idx = getMinIdx(q, num_seqs * num_seqs);
    int idx1 = idx / num_seqs;
    int idx2 = idx % num_seqs;
    if (idx1 > idx2) {
      swap(idx1, idx2);
    }
    cout << idx1 << ", " << idx2 << "\n";
    float length = a[idx1][idx2];
    float branch_length1 = length / 2 + (s[idx1] - s[idx2]) / 2;
    float branch_length2 = length - branch_length1;
    root = new Tree(nodes[idx1]->num_nodes + nodes[idx2]->num_nodes, length / 2,
                    nodes[idx1], nodes[idx2], branch_length1, branch_length2);
    update((float *)a, num_seqs, idx1, idx2);

    for (int i = 0; i < num_seqs; ++i) {
      for (int j = 0; j < num_seqs; ++j) {
	cout << a[i][j] << ",\t";
      }
      cout << "\n";
    }
    cout << "--------------------------------------\n";
    nodes[idx1] = root;
  }

  // Print the Tree
  printTree(root);

  free(q);
  cleanupTree(root);

  return 0;
}
