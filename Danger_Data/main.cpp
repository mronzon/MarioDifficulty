
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

void propagate(const cv::Mat& collision_filled, cv::Mat& danger_gradient_temp, const cv::Point& point) {
	if (point.x < 0 || point.y < 0 || point.y >= collision_filled.cols) return;
	if (collision_filled.at<uchar>(point.x, point.y) > 0) return;
	if (danger_gradient_temp.at<uchar>(point.x, point.y) != 0) return;

	danger_gradient_temp.at<uchar>(point.x, point.y) = 255;
	propagate(collision_filled, danger_gradient_temp, cv::Point(point.x - 1, point.y - 1));
	propagate(collision_filled, danger_gradient_temp, cv::Point(point.x - 1, point.y)    );
	propagate(collision_filled, danger_gradient_temp, cv::Point(point.x - 1, point.y + 1));
	return;
}

int main(int argc, char* argv[]) {
	if (argc != 2) return 0;
	std::string base_path = argv[1];

	const cv::Mat collision_filled = cv::imread(base_path + "\\collision_filled.png", cv::IMREAD_GRAYSCALE);
	cv::Mat danger_filled(collision_filled.rows, collision_filled.cols, CV_8UC1, cv::Scalar(0));

	for (int y = 0; y < danger_filled.cols; y++) {
		propagate(collision_filled, danger_filled, cv::Point(danger_filled.rows - 1, y));
		printf("progress...%i%\n", 100 * (y + 1) / danger_filled.cols);
	}
	cv::imwrite(base_path + "\\danger_filled.png", danger_filled);

	return 0;
}