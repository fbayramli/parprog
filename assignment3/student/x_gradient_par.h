#ifndef _X_GRADIENT_PAR
#define _X_GRADIENT_PAR

#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>
#include <thread>
#include <future>
#include <omp.h>
#include "x_gradient.h"

using namespace boost::gil;

template <typename Out> struct halfdiff_cast_channels; // forward declaration

template <typename SrcView, typename DstView>
void graders(int id, const SrcView& src, const DstView& dst, int num_threads)
{
    typedef typename channel_type<DstView>::type dst_channel_t;

      int start = src.height()/num_threads*id;
      int stop = src.height()/num_threads*(id+1);

      if (id == (num_threads - 1))
        stop = src.height();

    for(int y = start; y < stop; ++y)
    {
        typename SrcView::x_iterator src_it = src.row_begin(y);
        typename DstView::x_iterator dst_it = dst.row_begin(y);

        for (int x = 1; x < src.width() - 1; ++x)
        {
            static_transform(src_it[x - 1], src_it[x + 1], dst_it[x],
                             halfdiff_cast_channels<dst_channel_t>());
        }
    }
}

template <typename SrcView, typename DstView>

void x_gradient(const SrcView& src, const DstView& dst, int num_threads){
  std::thread t[200];
  for(int i = 0; i < num_threads; ++i){
    t[i] = std::thread(graders< SrcView, DstView>, i, std::cref(src), std::cref(dst), num_threads);
  }
  for(int i = 0; i<num_threads; ++i){
    t[i].join();
  }
}

#endif // !_X_GRADIENT_PAR_
