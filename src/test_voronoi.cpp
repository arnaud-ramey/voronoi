/*!
  \file        test_voronoi.cpp
  \author      Arnaud Ramey <arnaud.a.ramey@gmail.com>
                -- Robotics Lab, University Carlos III of Madrid
  \date        2013/9/19

________________________________________________________________________________

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
________________________________________________________________________________

Some tests for VoronoiThinner

 */
#include <gtest/gtest.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> // for erode
#include "timer.h"

#include "voronoi.h"

//int codec = CV_FOURCC('M', 'P', '4', '2');
int codec = CV_FOURCC('M', 'J', 'P', 'G');
// from http://opencv.willowgarage.com/wiki/VideoCodecs
//int codec = CV_FOURCC('I', '4', '2', '0'); // uncompressed
//int codec = 0;

class VoronoiIterator {
public:
  VoronoiIterator() {}

  void init(const cv::Mat1b & query,
            std::string implementation_name_,
            bool crop_img_before_) {
    _implementation_name = implementation_name_;
    _crop_img_before = crop_img_before_;
    _first_img = query.clone();
    VoronoiThinner::copy_bounding_box_plusone(query, _first_img, true);
    _curr_skel = _first_img.clone();
    // printf("first_img:%s\n", image_utils::infosImage(_first_img).c_str());
    _curr_iter = 1;
    _nframes = 0;
  }

  inline cv::Mat1b first_img() {
    return _first_img.clone();
  }

  cv::Mat1b current_skel() const {
    return _thinner.get_skeleton().clone();
  }

  inline cv::Mat1b contour_brighter(const cv::Mat1b & img) {
    _contour_viz.from_image_C4(img);
    cv::Mat1b ans;
    _contour_viz.copyTo(ans);
    return ans;
  }

  inline cv::Mat3b contour_color(const cv::Mat1b & img) {
    _contour_viz.from_image_C4(img);
    return _contour_viz.illus().clone();
  }

  //! \return true if success
  bool iter() {
    ++_nframes;
    bool reuse = (_implementation_name != IMPL_MORPH); // cant reuse with morph implementation
    bool success = false;
    if (reuse)
      success = _thinner.thin(_curr_skel, _implementation_name, false, 1); // already cropped
    else
      success = _thinner.thin(_first_img, _implementation_name, false, _nframes); // already cropped
    _thinner.get_skeleton().copyTo(_curr_skel);
    return success;
  }

  inline bool has_converged() const { return _thinner.has_converged(); }

  inline int cols() const { return _first_img.cols; }
  inline int rows() const { return _first_img.rows; }

  //protected:
  std::string _implementation_name;
  bool _crop_img_before;
  int _nframes;
  int _curr_iter;
  cv::Mat1b _first_img;
  cv::Mat1b _curr_skel;
  VoronoiThinner _thinner;
  ImageContour _contour_viz;
}; // end class VoronoiIterator

////////////////////////////////////////////////////////////////////////////////

void generate_video_bw(const cv::Mat1b & query,
                       std::string implementation_name,
                       bool crop_img_before,
                       bool show_contour_brighter = false) {
  // init iterator
  VoronoiIterator it;
  it.init(query, implementation_name, crop_img_before);

  // video writer
  // for a weird reason, odd width and height result in an empty video
  cv::Size output_size(it.cols() * 2, it.rows() * 2);
  std::ostringstream out_filename;
  out_filename << implementation_name << "_bw.avi";
  cv::VideoWriter writer(out_filename.str(), codec, 20,
                         output_size, false); // BW
  assert(writer.isOpened());
  cv::Mat1b curr_resized;
  cv::resize((show_contour_brighter ? it.contour_brighter(it.first_img()) : it.first_img()),
             curr_resized, output_size, CV_INTER_NN);
  writer << curr_resized;
  cv::imshow("curr_resized", curr_resized); cv::waitKey(10);

  // loop
  while (!it.has_converged()) {
    it.iter();
    cv::resize((show_contour_brighter ? it.contour_brighter(it.current_skel()) : it.current_skel()),
               curr_resized, output_size, CV_INTER_NN);
    writer << curr_resized;
    cv::imshow("curr_resized", curr_resized); cv::waitKey(10);
  }
  printf("generated '%s' (%i frames, %ix%i)\n",
         out_filename.str().c_str(), it._nframes, output_size.width, output_size.height);
}

////////////////////////////////////////////////////////////////////////////////

// from drawing_utils.h
template<class T>
void paste_images_gallery(const std::vector<cv::Mat_<T> > & in,
                cv::Mat_<T> & out,
                int gallerycols, T background_color,
                bool draw_borders = false, cv::Scalar border_color = CV_RGB(0,0,0)) {
  if (in.size() == 0) {
    out.create(0, 0);
    return;
  }
  int cols1 = in[0].cols, rows1 = in[0].rows, nimgs = in.size();
  // prepare output
  int galleryrows = std::ceil(1. * nimgs / gallerycols);
  out.create(rows1 * galleryrows, cols1 * gallerycols);
  // printf("nimgs:%i, gallerycols:%i, galleryrows:%i\n", nimgs, gallerycols, galleryrows);
  out.setTo(background_color);
  // paste images
  for (int img_idx = 0; img_idx < nimgs; ++img_idx) {
    int galleryx = img_idx % gallerycols, galleryy = img_idx / gallerycols;
    cv::Rect roi(galleryx * cols1, galleryy * rows1, cols1, rows1);
    // printf("### out:%ix%i, roi %i:'%s'\n", out.cols, out.rows, img_idx, geometry_utils::print_rect(roi).c_str());
    if (cols1 != in[img_idx].cols || rows1 != in[img_idx].rows) {
      printf("Image %i of size (%ix%i), different from (%ix%i), skipping it.\n",
             img_idx, in[img_idx].cols, in[img_idx].rows, cols1, rows1);
      cv::line(out, roi.tl(), roi.br(), border_color, 2);
      cv::line(out, roi.br(), roi.tl(), border_color, 2);
    }
    else {
      cv::Mat_<T> out_roi = out(roi);
      in[img_idx].copyTo( out_roi );
    }
    if (draw_borders)
      cv::rectangle(out, roi, border_color, 1);
  } // end loop img_idx
} // end paste_images_gallery<_T>

////////////////////////////////////////////////////////////////////////////////

void generate_video_comparer(const cv::Mat1b & query,
                             bool crop_img_before,
                             int gallerycols,
                             const std::string out_prefix = "comparer_") {
  // init iterators
  static const unsigned int nimpls = 3;
  std::string impls[] = {IMPL_MORPH, IMPL_ZHANG_SUEN_FAST, IMPL_GUO_HALL_FAST};
  cv::Mat3b curr, curr_resized;
  VoronoiIterator its[nimpls];
  for (unsigned int impl_idx = 0; impl_idx < nimpls; ++impl_idx)
    its[impl_idx].init(query, impls[impl_idx], crop_img_before);
  cv::Mat3b first_img_color;
  // first_img_color = its[0].contour_color(its[0].first_img());
  cv::cvtColor(its[0].first_img(), first_img_color, CV_GRAY2BGR);
  int /*cols1 = first_img_color.cols, */ rows1 = first_img_color.rows;
  //cv::Point text_pos(cols1 / 2, rows1 - 10);
  // image_utils::draw_text_centered
  cv::Point text_pos(10, rows1 - 10);
  cv::putText(first_img_color, "query",
              text_pos, CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0, 255, 0));

  // video writer
  // for a weird reason, odd width and height result in an empty video
  cv::Size output_size
      (gallerycols * 2 * its[0].cols(), ceil(4 / gallerycols) * 2 * its[0].rows());
  unsigned int fps = 5;
  std::ostringstream out_filename;
  out_filename << out_prefix << ".avi";
  cv::VideoWriter writer(out_filename.str(), codec, fps, output_size, true); // color
  assert(writer.isOpened());
  unsigned int n_terminated = 0, iters = 0;
  while(n_terminated < nimpls) {
    ++iters;
    // iterate all implementations
    n_terminated = 0;
    std::vector<cv::Mat3b> curr_imgs;
    curr_imgs.push_back(first_img_color);
    for (unsigned int impl_idx = 0; impl_idx < nimpls; ++impl_idx) {
      if (its[impl_idx].has_converged())
        ++n_terminated;
      else
        its[impl_idx].iter();
      cv::Mat3b curr_img_color = its[impl_idx].contour_color(its[impl_idx].current_skel());
      cv::dilate(curr_img_color, curr_img_color, cv::Mat());
      //image_utils::draw_text_centered
      cv::putText
          (curr_img_color, impls[impl_idx],
           text_pos, CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0, 255, 0));
      curr_imgs.push_back(curr_img_color);
    } // end for impl_idx
    // now make collage
    paste_images_gallery(curr_imgs, curr, gallerycols, cv::Vec3b(0, 0, 0), true, CV_RGB(255, 255, 255));
    cv::resize(curr, curr_resized, output_size, CV_INTER_NN);
    writer << curr_resized;
    cv::imshow("curr", curr); cv::waitKey(10);
    // http://denislantsman.com/?p=111 - save as png too
    // convert: ffmpeg -i out_%d.png -vcodec png out.avi
    // http://stackoverflow.com/questions/4839303/convert-image-sequence-to-lossless-movie
    std::ostringstream name; name << "comparer_" << std::setprecision(3) << std::setfill('0') << iters << ".png";
    cv::imwrite(name.str(), curr);
  } // end while1
  // add the last image for twp second
  for (unsigned int i = 0; i < 2 * fps; ++i) {
    writer << curr_resized;
    std::ostringstream name; name << "comparer_" << std::setprecision(3) << std::setfill('0') << ++iters << ".png";
    cv::imwrite(name.str(), curr);
  } // end loop i

  std::ostringstream out_filename_lossless;
  out_filename_lossless << out_prefix << "_lossless.avi";
  std::ostringstream command;
  command << "ffmpeg -r " << fps << " -y -i comparer_%d.png "
          << " -vcodec png "
             //<< " -vcodec huffyuv "
             // << " -vcodec mjpeg "
          << out_filename_lossless.str() << "; rm comparer_*.png";
  system(command.str().c_str());

  printf("generated '%s' and '%s' (%i frames, %ix%i)\n",
         out_filename.str().c_str(), out_filename_lossless.str().c_str(),
         iters, output_size.width, output_size.height);
} // end generate_video_comparer();

////////////////////////////////////////////////////////////////////////////////

void benchmark(const cv::Mat1b & query,
               bool display_imgs = true) {
  VoronoiThinner thinner;
  unsigned int ntimes = 10;
  Timer timer;
  std::vector<std::string> impls = VoronoiThinner::all_implementations();
  std::vector<cv::Mat1b> skels_no_crop;
  std::vector<cv::Mat1b> skels_crop;
  printf("Running benchmark with implementations [%s]...\n",
         VoronoiThinner::all_implementations_as_string().c_str());
  for (unsigned int i = 0; i < impls.size(); ++i) {
    for (unsigned int crop = 0; crop <= 1; ++crop) {
      timer.reset();
      for (unsigned int time = 0; time < ntimes; ++time)
        thinner.thin(query, impls[i], crop);
      printf("Time for thin('%s', crop:%i):\t %g ms\n",
             impls[i].c_str(), crop, timer.getTimeMilliseconds() / ntimes);
      if (crop)
        skels_crop.push_back(thinner.get_skeleton().clone());
      else
        skels_no_crop.push_back(thinner.get_skeleton().clone());
    } // end loop crop
  } // end loop i

  if (!display_imgs)
    return;
  cv::imshow("query", query);
  //  printf("impls:%i, skels_no_crop:%i, skels_crop:%i\n",
  //         impls.size(), skels_no_crop.size(), skels_crop.size());
  for (unsigned int i = 0; i < impls.size(); ++i) {
    cv::imshow(impls[i] + std::string("nocrop"), skels_no_crop[i]);
    cv::imshow(impls[i] + std::string("crop"), skels_crop[i]);
  } // end loop i
  cv::waitKey(0);
} // emd benchmark();

////////////////////////////////////////////////////////////////////////////////

void benchmark_logs(const cv::Mat1b & query) {
  unsigned int npixels_curr = query.cols * query.rows;
  VoronoiThinner thinner;
  std::vector<std::string> implementation_names;
  implementation_names.push_back(IMPL_ZHANG_SUEN);
  implementation_names.push_back(IMPL_ZHANG_SUEN_FAST);
  implementation_names.push_back(IMPL_GUO_HALL);
  implementation_names.push_back(IMPL_GUO_HALL_FAST);
  std::cout << "npixels \t";
  for (unsigned int imp_idx = 0; imp_idx < implementation_names.size(); ++imp_idx)
    std::cout << implementation_names[imp_idx] << " \t";
  std::cout << std::endl;

  for (unsigned int npixels = 100; npixels <= 1E7; npixels*=10) {
    cv::Mat1b query_resized;
    // npixels = w * ratio * h * ratio = npixels_curr * ratio^2 -> ratio = sqrt(npixels/npixels_curr)
    double ratio = sqrt(1. * npixels / npixels_curr);
    // printf("npixels:%i, npixels_curr:%i, ratio:%g\n", npixels, npixels_curr, ratio);
    cv::resize(query, query_resized, cv::Size(), ratio, ratio, CV_INTER_NN);
    // use all thinning algos
    unsigned int ntimes = 1;
    std::cout << npixels << " \t";
    for (unsigned int imp_idx = 0; imp_idx < implementation_names.size(); ++imp_idx) {
      Timer timer;
      for (unsigned int i = 0; i < ntimes; ++i)
        thinner.thin(query_resized, implementation_names[imp_idx], true);
      std::cout << timer.getTimeMilliseconds() / ntimes << " \t";
    } // end loop imp_idx
    std::cout << std::endl;
  } // end loop npixels
} // end benchmark_logs();

////////////////////////////////////////////////////////////////////////////////

inline int CLI_help(int argc, char** argv) {
  printf("Usage: %s <command> <implementation_name> <files>\n", argv[0]);
  printf(" * command: [ thin | video | video_bright | video_comparer | benchmark ]\n");
  printf("   If command =  video_comparer or benchmark, no implementation must be specified.\n");
  printf(" * implementation_name: [%s]\n",
         VoronoiThinner::all_implementations_as_string().c_str());
  printf("\nExamples:\n");
  printf("  %s video           morph            horse.png\n", argv[0]);
  printf("  %s thin            zhang_suen_fast  *.png\n", argv[0]);
  printf("  %s video_comparer                   *.png\n", argv[0]);
  return -1;
}

enum {THIN, VIDEO, VIDEO_BRIGHT, VIDEO_COMPARER, BENCHMARK};
int CLI(int argc, char** argv) {
  //  for (int argi = 0; argi < argc; ++argi)
  //    printf("argv[%i]:'%s'\n", argi, argv[argi]);
  if (argc < 3) // [exename] + 2 args
    return CLI_help(argc, argv);
  // detect order
  int order = -1;
  std::string order_str (argv[1]);
  if (order_str == "thin")
    order = THIN;
  else if (order_str == "video")
    order = VIDEO;
  else if (order_str == "video_bright")
    order = VIDEO_BRIGHT;
  else if (order_str == "video_comparer")
    order = VIDEO_COMPARER;
  else if (order_str == "benchmark")
    order = BENCHMARK;
  else {
    printf("Unknown order '%s'\n", order_str.c_str());
    return CLI_help(argc, argv);
  }
  // check implementation
  std::string implementation_name (argv[2]);
  int first_file_idx = 2;
  if (order != VIDEO_COMPARER && order != BENCHMARK) {
    first_file_idx = 3;
    if (!VoronoiThinner::is_implementation_valid(implementation_name)) {
      printf("Unknown implementation '%s'\n", implementation_name.c_str());
      return CLI_help(argc, argv);
    }
  }
  if (argc < first_file_idx + 1) // [exename] [order] [file(s)]
    return CLI_help(argc, argv);

  VoronoiThinner thinner;
  // load files
  std::vector<cv::Mat1b> files;
  for (int argi = first_file_idx; argi < argc; ++argi) {
    cv::Mat1b file = cv::imread(argv[argi], CV_LOAD_IMAGE_GRAYSCALE);
    if (file.empty())
      printf("Could not load file '%s'\n", argv[argi]);
    else
      files.push_back(file);
  } // end loop argi

  // process
  if (order == THIN) {
    for (unsigned int file_idx = 0; file_idx < files.size(); ++file_idx) {
      Timer timer;
      bool ok = thinner.thin(files[file_idx], implementation_name, true);
      if (!ok) {
        printf("Failed thinning with implementation '%s'\n", implementation_name.c_str());
        continue;
      }
      timer.printTime(implementation_name.c_str());
      // write file
      std::ostringstream out; out << "out_" << file_idx << ".png";
      cv::imwrite(out.str(), thinner.get_skeleton());
      printf("Written file '%s'\n", out.str().c_str());
      // show res
      cv::imshow("query", files[file_idx]);
      cv::imshow(implementation_name, thinner.get_skeleton());
      cv::waitKey(0);
    } // end loop file_idx
  } // end if (order == THIN)

  else if (order == VIDEO || order == VIDEO_BRIGHT) {
    for (unsigned int file_idx = 0; file_idx < files.size(); ++file_idx)
      generate_video_bw(files[file_idx], implementation_name,
                        true, (order == VIDEO_BRIGHT));
  } // end if (order == VIDEO || order == VIDEO_BRIGHT)


  else if (order == VIDEO_COMPARER) {
    for (unsigned int file_idx = 0; file_idx < files.size(); ++file_idx) {
      std::ostringstream out; out << "out_" << file_idx;
      generate_video_comparer(files[file_idx], true, 2, out.str());
    } // end loop file_idx
  } // end if (order == VIDEO_COMPARER)

  else if (order == BENCHMARK) {
    for (unsigned int file_idx = 0; file_idx < files.size(); ++file_idx)
      benchmark(files[file_idx], true);
  } // end if (order == BENCHMARK)

  return 0;
} // end CLI()

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
  // benchmark_logs(cv::imread(IMG_DIR "depth/juggling1_user_mask.png", CV_LOAD_IMAGE_GRAYSCALE));

  //  generate_video_bw(cv::imread(IMG_DIR "depth/juggling1_user_mask.png", CV_LOAD_IMAGE_GRAYSCALE),
  //                    IMPL_GUO_HALL, true, true);
  //  return 0;

  //  generate_video_bw(cv::imread(IMG_DIR "depth/juggling1_user_mask.png", CV_LOAD_IMAGE_GRAYSCALE),
  //                    IMPL_MORPH, true, true);
  //  return 0;

  //  generate_video_comparer(cv::imread(IMG_DIR "skeletons/horse.png", CV_LOAD_IMAGE_GRAYSCALE),
  //                          true, 2, "horse_");
  //  generate_video_comparer(cv::imread(IMG_DIR "depth/juggling1_user_mask.png", CV_LOAD_IMAGE_GRAYSCALE),
  //                          true, 4, "comparer_juggling1_mask_");
  //  generate_video_comparer(cv::imread(IMG_DIR "depth/alberto1_user_mask.png", CV_LOAD_IMAGE_GRAYSCALE),
  //                          true, 4, "alberto1_mask_");
  //  generate_video_comparer(cv::imread(IMG_DIR "skeletons/japanese_src.png", CV_LOAD_IMAGE_GRAYSCALE),
  //                          true, 2, "comparer_japanese_src_");
  //  generate_video_comparer(cv::imread(IMG_DIR "skeletons/opencv_src.png", CV_LOAD_IMAGE_GRAYSCALE),
  //                          true, 1, "opencv_src_");
  //  generate_video_comparer(cv::imread(IMG_DIR "powerXML/power.png", CV_LOAD_IMAGE_GRAYSCALE),
  //                          true, 2, "comparer_power_");
  return CLI(argc, argv);
}
