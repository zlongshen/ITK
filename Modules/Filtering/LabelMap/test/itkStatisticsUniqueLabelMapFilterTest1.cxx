/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"


#include "itkLabelImageToStatisticsLabelMapFilter.h"
#include "itkStatisticsUniqueLabelMapFilter.h"

#include "itkTestingMacros.h"

#include "itkFlatStructuringElement.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkObjectByObjectLabelMapFilter.h"

int itkStatisticsUniqueLabelMapFilterTest1(int argc, char * argv[])
{
  if( argc != 6 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " input feature output";
    std::cerr << " reverseOrdering attribute";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int dim = 2;

  typedef unsigned char PixelType;

  typedef itk::Image< PixelType, dim > ImageType;

  typedef itk::StatisticsLabelObject< PixelType, dim >           StatisticsLabelObjectType;
  typedef itk::LabelMap< StatisticsLabelObjectType >             LabelMapType;

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName( argv[2] );


  // Dilate each label object to form overlapping label objects.
  const unsigned int radiusValue = 5;

  typedef itk::LabelImageToLabelMapFilter< ImageType, LabelMapType > LabelImageToLabelMapFilterType;
  LabelImageToLabelMapFilterType::Pointer labelMapConverter = LabelImageToLabelMapFilterType::New();
  labelMapConverter->SetInput( reader->GetOutput() );
  labelMapConverter->SetBackgroundValue( itk::NumericTraits< PixelType >::Zero );

  typedef itk::FlatStructuringElement< dim > StructuringElementType;
  StructuringElementType::RadiusType radius;
  radius.Fill( radiusValue );

  StructuringElementType structuringElement = StructuringElementType::Ball( radius );

  typedef itk::GrayscaleDilateImageFilter< ImageType, ImageType, StructuringElementType > MorphologicalFilterType;
  MorphologicalFilterType::Pointer grayscaleDilateFilter = MorphologicalFilterType::New();
  grayscaleDilateFilter->SetInput( reader->GetOutput() );
  grayscaleDilateFilter->SetKernel( structuringElement );

  typedef itk::ObjectByObjectLabelMapFilter< LabelMapType > ObjectByObjectLabelMapFilterType;
  ObjectByObjectLabelMapFilterType::Pointer objectByObjectLabelMapFilter = ObjectByObjectLabelMapFilterType::New();
  objectByObjectLabelMapFilter->SetInput( labelMapConverter->GetOutput() );
  objectByObjectLabelMapFilter->SetBinaryInternalOutput( false );
  objectByObjectLabelMapFilter->SetFilter( grayscaleDilateFilter );

  typedef itk::StatisticsLabelMapFilter< LabelMapType, ImageType > I2LType;
  I2LType::Pointer i2l = I2LType::New();
  i2l->SetInput1( objectByObjectLabelMapFilter->GetOutput()  );
  i2l->SetFeatureImage( reader2->GetOutput() );

  typedef itk::StatisticsUniqueLabelMapFilter< LabelMapType > LabelUniqueType;
  LabelUniqueType::Pointer Unique = LabelUniqueType::New();

  //testing boolean macro for ReverseOrdering
  Unique->ReverseOrderingOn();
  TEST_SET_GET_VALUE( true, Unique->GetReverseOrdering() );

  Unique->ReverseOrderingOff();
  TEST_SET_GET_VALUE( false, Unique->GetReverseOrdering() );

  //testing get and set macros for ReverseOrdering
  bool reverseOrdering = atoi( argv[4] );
  Unique->SetReverseOrdering( reverseOrdering );
  TEST_SET_GET_VALUE( reverseOrdering , Unique->GetReverseOrdering() );


  //testing get and set macros for Attribute
  LabelUniqueType::AttributeType attribute = atoi( argv[5] );
  Unique->SetAttribute( attribute );
  TEST_SET_GET_VALUE( attribute, Unique->GetAttribute() );

  Unique->SetInput( i2l->GetOutput() );

  itk::SimpleFilterWatcher watcher(Unique, "filter");

  typedef itk::LabelMapToLabelImageFilter< LabelMapType, ImageType> L2IType;
  L2IType::Pointer l2i = L2IType::New();
  l2i->SetInput( Unique->GetOutput() );

  typedef itk::ImageFileWriter< ImageType > WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( l2i->GetOutput() );
  writer->SetFileName( argv[3] );
  writer->UseCompressionOn();

  TRY_EXPECT_NO_EXCEPTION( writer->Update() );

  return EXIT_SUCCESS;
}
