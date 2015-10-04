#ifndef DESPECKCL_H
#define DESPECKCL_H

#include <vector>
#include <string>

namespace despeckcl {

    void boxcar(float* ampl_master,
                float* ampl_slave,
                float* dphase,
                float* ampl_filt,
                float* dphase_filt,
                float* coh_filt,
                const int height,
                const int width,
                const int window_width,
                std::vector<std::string> enabled_log_levels);

    int nlsar(float* ampl_master,
              float* ampl_slave,
              float* dphase,
              float* ampl_filt,
              float* dphase_filt,
              float* coh_filt,
              const int height,
              const int width,
              const int search_window_size,
              const std::vector<int> patch_sizes,
              const std::vector<int> scale_sizes,
              std::vector<std::string> enabled_log_levels);

    int nlinsar(float* ampl_master,
                float* ampl_slave,
                float* dphase,
                float* ampl_filt,
                float* dphase_filt,
                float* coh_filt,
                const int height,
                const int width,
                const int search_window_size,
                const int patch_size,
                const int niter,
                const int lmin,
                std::vector<std::string> enabled_log_levels);

}

#endif
