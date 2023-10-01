#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

// TODO Write this function
int energy(const Pixel *const*image, int col, int row, int width, int height)
{

  int xminus = col - 1; // default pixel to left
  int xplus = col + 1; // default pixel to right
  int yminus = row - 1; // default row above (-y direction in this case) 
  int yplus = row + 1; // default row below (+y direction in this case)
  if (col == 0) {xminus = width - 1;} // if on left edge make the pixel to the left be the pixel furthest to the right
  if (col == (width-1)) {xplus = 0;} // if on right edhe make the pixel to the right the left most pixel
  if (row == 0) {yminus = height - 1;} // if on the top edge make the pixel above the lowest pixel
  if (row == (height-1)) {yplus = 0;} // if on the bottom edge make the pixel below the highest pixel

  int redX = image[xplus][row].r - image[xminus][row].r; // Rx
  int greenX = image[xplus][row].g - image[xminus][row].g; // Gx
  int blueX = image[xplus][row].b - image[xminus][row].b; // Bx
  int energyX = pow(redX, 2) + pow(greenX, 2) + pow(blueX, 2); // Ex = Rx^2 + Gx^2 + Bx^2
  int redY = image[col][yplus].r - image[col][yminus].r; // Ry
  int greenY = image[col][yplus].g - image[col][yminus].g; // Gy
  int blueY = image[col][yplus].b - image[col][yminus].b; // By
  int energyY = pow(redY, 2) + pow(greenY, 2) + pow(blueY, 2); // Ey = Ry^2 + Gy^2 + By^2
  int energy = energyX + energyY; // Ex + Ey

  return energy;
}

// TODO Write this function
int getVerticalSeam(const Pixel *const*image, int start_col, int width, int height, int* seam)
{
  int tempCol = start_col; // creating a value to keep track of the column without changing inputted variable just for good practice purposes
  int totalEnergy = energy(image, tempCol, 0, width, height); // starting total energy value using the initial starting coordinate of (start_col, )
  int right, forward, left; // declare variables 
  seam[0] = tempCol; // setting the first value of index i to the starting column , seam array follows the format where the index = y and the value = x
  //cout << "(x" << seam[0] << ", y"<< 0 << ")" << endl; // test line to print the starting coordinate of the seam array
  for (int i = 1; i < height; ++i) {
    if (tempCol > 0 && tempCol < width - 1) { // if it is in the middle 
      right = energy(image, tempCol - 1, i, width, height); // left from our pov but right from pc pov -- column deiterate 
      forward = energy(image, tempCol, i, width, height); // effectively going down and not changing column
      left = energy(image, tempCol + 1, i, width, height); // right from our pov but left from pc pov -- column iterate
      //cout << "right " << right << " left " << left << " forward " << forward << endl;
    }
    else if (tempCol == 0) { // if the current column is on the left from our POV but right from computer POV
      right = 400000; // int value significantly greater than that of the max energy for a pixel 
      forward = energy(image, tempCol, i, width, height); // effectively going down and not changing column
      left = energy(image, tempCol + 1, i, width, height); // right from our pov but left from pc pov -- column iterate
      //cout << "right " << right << " left " << left << " forward " << forward << " col = 0 " << endl;
    }
    else if (tempCol == width - 1) { // if the current column is on the right from our POV but left from computer POV
      right = energy(image, tempCol - 1, i, width, height); // left from our pov but right from pc pov -- column deiterate
      forward = energy(image, tempCol, i, width, height); // effectively going down and not changing column
      left = 400000; // int value significantly greater than that of the max energy for a pixel
      //cout << "right " << right << " left " << left << " forward " << forward << " col = max " << endl;
    }
    if (forward <= right && forward <= left) { // First prefer to go directly forward (i.e. keep the same column) (⬇ in context of the table)
      //cout << "forward " << endl; // test line
      totalEnergy += forward; // add the energy of the forward way to the total energy of the seam
      seam[i] = tempCol;
      // no iteration because column stays the same 
    }
    else if (left <= right && left < forward) { // Next prefer to go left (i.e. increase the column by 1) (⬊ in context of the table)
      //cout << "left " << endl; // test line 
      totalEnergy += left; // add the energy of the left way to the total energy of the seam
      seam[i] = tempCol + 1;
      ++tempCol; // iterate column
    }
    else if (right < forward && right < left) { // you will only go right (i.e. decrease the column by 1) when it is strictly less than other options.
      totalEnergy += right; // add the energy of the right way to the total energy of the seam
      //cout << "right " << endl; // test line
      seam[i] = tempCol - 1; 
      --tempCol; // deiterate column
      
    } 
    //cout << " epic " << endl;
    seam[i] = tempCol;
    //cout << "(x" << seam[i] << ", y"<< i << ")" << endl; // test line to get values of the seam and its route 
  }
  //cout << "total energy " << totalEnergy << endl; // test line to find the total energy of the vertical seam
  
  return totalEnergy; // return the total energy of the vertical seam
}

// TODO Write this function
void removeVerticalSeam(Pixel **image, int& width, int height, int *verticalSeam)
{
  bool replace; // variable to determine whether to start replacing values or not
  for (int y = 0; y < height; ++y) { // nested for loop with outermost being height so that x is taken care of for every y as the height will not be decreasing but the width will be 
    replace = false; // make sure replace = false at start of each iteration
    for (int x = 0; x < width; ++x) { // inner loop for width 
      if (x == verticalSeam[y]) { // once the x val == the value from the index of y (the column of the row in vert seam) then start replacing
        replace = true; // will hold true until the y iterates
      }
      if (replace && x < (width - 1)) {
        image[x][y] = image[x + 1][y];
        
      }
    }
  }
  //delete[] verticalSeam; // deallocate verticalSeam ??
  //verticalSeam = nullptr;
  --width; // deiterate width to compensate for the removed pixel
}

// TODO Write this function for extra credit
int getHorizontalSeam(const Pixel *const*image, int start_row, int width, int height, int* seam)
{
  /* if (image == nullptr) {;}
  if (start_row == 0) {;}
  if (width == 0) {;}
  if (height == 0) {;}
  if (seam == nullptr) {;} */
  int tempRow = start_row; // creating a value to keep track of the column without changing inputted variable just for good practice purposes
  int totalEnergy = energy(image, 0, tempRow, width, height); // starting total energy value using the initial starting coordinate of (start_col, )
  int right, forward, left; // declare variables 
  seam[0] = tempRow; // setting the first value of index i to the starting column , seam array follows the format where the index = y and the value = x
  //cout << "(x" << seam[0] << ", y"<< 0 << ")" << endl; // test line to print the starting coordinate of the seam array
  for (int i = 1; i < width; ++i) {
    if (tempRow > 0 && tempRow < height - 1) { // if it is in the middle 
      left = energy(image, i, tempRow - 1, width, height); // left from our pov but right from pc pov -- column deiterate 
      forward = energy(image, i, tempRow, width, height); // effectively going down and not changing column
      right = energy(image, i, tempRow + 1, width, height); // right from our pov but left from pc pov -- column iterate
      cout << "left " << left << " forward " << forward << " right " << right << endl;
    }
    else if (tempRow == 0) { // if the current column is on the left from our POV but right from computer POV
      left = 400000; // int value significantly greater than that of the max energy for a pixel 
      forward = energy(image, i, tempRow, width, height); // effectively going down and not changing column
      right = energy(image, i, tempRow + 1, width, height); // right from our pov but left from pc pov -- column iterate
      //cout << "right " << right << " left " << left << " forward " << forward << " col = 0 " << endl;
    }
    else if (tempRow == height - 1) { // if the current column is on the right from our POV but left from computer POV
      left = energy(image, i, tempRow - 1, width, height); // left from our pov but right from pc pov -- column deiterate
      forward = energy(image, i, tempRow, width, height); // effectively going down and not changing column
      right = 400000; // int value significantly greater than that of the max energy for a pixel
      //cout << "right " << right << " left " << left << " forward " << forward << " col = max " << endl;
    }
    if (left <= right && left <= forward) { // First prefer to go directly forward (i.e. keep the same column) (⬇ in context of the table)
      //cout << "left " << endl; // test line
      totalEnergy += left; // add the energy of the forward way to the total energy of the seam
      seam[i] = tempRow - 1;
      --tempRow;
      // no iteration because column stays the same 
    }
    else if (forward <= right && forward < left) { // Next prefer to go left (i.e. increase the column by 1) (⬊ in context of the table)
      //cout << "forward " << endl; // test line 
      totalEnergy += forward; // add the energy of the left way to the total energy of the seam
      seam[i] = tempRow;
       // iterate column
    }
    else if (right < forward && right < left) { // you will only go right (i.e. decrease the column by 1) when it is strictly less than other options.
      totalEnergy += right; // add the energy of the right way to the total energy of the seam
      //cout << "right " << endl; // test line
      seam[i] = tempRow + 1; 
      ++tempRow; // deiterate column
      
    } 
    //cout << " epic " << endl;
    seam[i] = tempRow;
  }
  return totalEnergy;
}

// TODO Write this function for extra credit
void removeHorizontalSeam(Pixel **image, int width, int& height, int *horizontalSeam)
{
  if (image == nullptr) {;}
  if (width == 0) {;}
  if (height == 0) {;}
  if (horizontalSeam == nullptr) {;}
}

int *findMinVerticalSeam(const Pixel *const*image, int width, int height)
{
  // initialize minSeam and minDistance to seam starting at first col (index 0)
  int *minSeam = new int[height]{0};
  int minDist = getVerticalSeam(image, 0, width, height, minSeam);

  int *candidateSeam = new int[height]{0};
  int candidateDistance = -1; // invalid distance

  // start at second col (index 1) since we initialized with first col (index 0)
  for (int col = 1; col < width; ++col)
  {
    candidateDistance = getVerticalSeam(image, col, width, height, candidateSeam);

    if (candidateDistance < minDist)
    { // new min
      //  swap min & candidate
      minDist = candidateDistance;
      int* temp = candidateSeam;
      candidateSeam = minSeam;
      minSeam = temp;
    }
  }

  // clean up 
  delete [] candidateSeam;

  return minSeam;
}

/* int *findMinHorizontalSeam(const Pixel *const*image, int width, int height)
{
  // initialize minSeam and minDistance to seam starting at first row (index 0)
  int *minSeam = new int[width]{0};
  int minDistance = getHorizontalSeam(image, 0, width, height, minSeam);

  int *candidateSeam = new int[width]{0};
  int candidateDistance = -1; // invalid distance

  // start at second row (index 1) since we initialized with first row (index 0)
  for (int row = 1; row < height; ++row)
  {
    candidateDistance = getHorizontalSeam(image, row, width, height, candidateSeam);

    if (candidateDistance < minDistance)
    { // new minimum
      //  swap min and candidate seams
      minDistance = candidateDistance;
      int* temp = candidateSeam;
      candidateSeam = minSeam;
      minSeam = temp;
    }
  }

    // clean up 
  delete [] candidateSeam;

  return minSeam;
} */

Pixel **createImage(int width, int height)
{
  cout << "Start createImage... " << endl;

  // Create a one dimensional array on the heap of pointers to Pixels
  //    that has width elements (i.e. the number of columns)
  Pixel **image = new Pixel *[width] {}; // initializes to nullptr

  for (int col = 0; col < width; ++col)
  { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    //  that has height elements (i.e. the number of rows)
    try
    {
      image[col] = new Pixel[height];
    }
    catch (std::bad_alloc &e)
    {
      // clean up already allocated arrays
      for (int i = 0; i < col; ++i)
      {
        delete[] image[i];
      }
      delete[] image;
      // rethrow
      throw e;
    }
  }

  // initialize cells
  // cout << "Initializing cells..." << endl;
  for (int row = 0; row < height; ++row)
  {
    for (int col = 0; col < width; ++col)
    {
      // cout << "(" << col << ", " << row << ")" << endl;
      image[col][row] = {0, 0, 0};
    }
  }
  cout << "End createImage... " << endl;
  return image;
}

void deleteImage(Pixel **image, int width)
{
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i = 0; i < width; ++i)
  {
    delete[] image[i];
  }
  delete[] image;
  image = nullptr;
  cout << "End deleteImage..." << endl;
}

bool isValidColor(int colorVal)
{
  if (colorVal < 0 || colorVal > 255)
  {
    return false;
  }
  return true;
}

Pixel ** loadImage(string filename, int &width, int &height)
{
  cout << "Start loadImage..." << endl;
  // remove
  ifstream ifs(filename);
  if (!ifs.is_open())
  {
    throw std::invalid_argument("Failed to open input file (" + filename + ")");
  }

  string type;
  ifs >> type; // should be P3
  if (toupper(type.at(0)) != 'P' || type.at(1) != '3')
  {
    throw std::domain_error("Not PPM type P3 (" + type + ")");
  }
  ifs >> width;
  // cout << "w and h: " << w << " " << h << endl;
  if (ifs.fail())
  {
    throw std::domain_error("Read non-integer value for width");
  }
  if (width <= 0)
  {
    ostringstream oss;
    oss << "Width in file must be greater than 0 (" << width << ")";
    throw std::domain_error(oss.str());
  }

  ifs >> height;
  if (ifs.fail())
  {
    cout << "Read non-integer value for height" << endl;
  }
  if (height <= 0)
  {
    ostringstream oss;
    oss << "Height in file must be greater than 0 (" << height << ")";
    throw std::domain_error(oss.str());
  }

  int colorMax = 0;
  ifs >> colorMax;
  if (ifs.fail())
  {
    throw std::domain_error("Read non-integer value for max color value");
  }
  if (colorMax != 255)
  {
    ostringstream oss;
    oss << "Max color value must be 255 (" << colorMax << ")";
    throw std::domain_error(oss.str());
  }

  // load image throws exceptions but we will let them pass through
  Pixel **image = createImage(width, height);

  for (int row = 0; row < height; ++row)
  {
    for (int col = 0; col < width; ++col)
    {
      // cout << "Pixel(" << col << ", " << row << ")" << endl;
      ifs >> image[col][row].r;
      if (ifs.fail() && !ifs.eof())
      { // bad input that is not caused by being at the end of the file
        throw std::domain_error("Read non-integer value for red");
      }
      if (!isValidColor(image[col][row].r))
      {
        ostringstream oss;
        oss << "Invalid color value for red (" << image[col][row].r << ")";
        throw std::domain_error(oss.str());
      }

      ifs >> image[col][row].g;
      if (ifs.fail() && !ifs.eof())
      { // bad input that is not caused by being at the end of the file
        throw std::domain_error("Read non-integer value for green");
      }
      if (!isValidColor(image[col][row].r))
      {
        ostringstream oss;
        oss << "Invalid color value for green (" << image[col][row].r + ")";
        throw std::domain_error(oss.str());
      }

      ifs >> image[col][row].b;
      if (ifs.fail() && !ifs.eof())
      { // bad input that is not caused by being at the end of the file
        throw std::domain_error("Read non-integer value for blue");
      }
      if (!isValidColor(image[col][row].r))
      {
        ostringstream oss;
        oss << "Invalid color value for blue (" << image[col][row].r + ")";
        throw std::domain_error(oss.str());
      }
    }
  }
  cout << "End loadImage..." << endl;
  return image;
}

void outputImage(string filename, const Pixel *const *image, int width, int height)
{
  cout << "Start outputImage..." << endl;
  // remove code
  // declare/define and open output file stream with filename
  ofstream ofs(filename);
  // ensure file is open
  if (!ofs.is_open())
  {
    throw std::invalid_argument("Error: failed to open output file - " + filename);
  }
  ofs << "P3" << endl;
  ofs << width << " " << height << endl;
  ofs << 255 << endl;
  for (int row = 0; row < height; ++row)
  {
    for (int col = 0; col < width; ++col)
    {
      ofs << image[col][row].r << " ";
      ofs << image[col][row].g << " ";
      ofs << image[col][row].b << " ";
      //cout << "col, row " << col << ", " << row << " r " << image[col][row].r << " g " << image[col][row].g << " b " << image[col][row].b << endl;
    }
    ofs << endl;
  }
  cout << "End outputImage..." << endl;
}