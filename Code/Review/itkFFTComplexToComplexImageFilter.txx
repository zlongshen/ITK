/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFFTComplexToComplexImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/**
 *
 * Attribution Notice. This research work was made possible by
 * Grant Number R01 RR021885 (PI Simon K. Warfield, Ph.D.) from
 * the National Center for Research Resources (NCRR), a component of the
 * National Institutes of Health (NIH).  Its contents are solely the
 * responsibility of the authors and do not necessarily represent the
 * official view of NCRR or NIH.
 *
 * This class was taken from the Insight Journal paper:
 * http://insight-journal.org/midas/handle.php?handle=1926/326
 *
 */
#ifndef __itkFFTComplexToComplexImageFilter_txx
#define __itkFFTComplexToComplexImageFilter_txx

#include "itkFFTComplexToComplexImageFilter.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"

namespace itk
{


template <class TPixel, unsigned int NDimension, int NDirection>
void
FFTComplexToComplexImageFilter<TPixel,NDimension,NDirection>::
GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();
   //
  // If this implementation returns a full result
  // instead of a 'half-complex' matrix, then none of this
  // is necessary
  if(this->FullMatrix())
    return;
 
  // get pointers to the input and output
  typename TInputImageType::ConstPointer  inputPtr  = this->GetInput();
  typename TOutputImageType::Pointer      outputPtr = this->GetOutput();

  if ( !inputPtr || !outputPtr )
    {
    return;
    }
  
  // 
  // This is all based on the same function in itk::ShrinkImageFilter
  // ShrinkImageFilter also modifies the image spacing, but spacing
  // has no meaning in the result of an FFT. For an IFFT, since the
  // spacing is propagated to the complex result, we can use the spacing
  // from the input to propagate back to the output.
  unsigned int i;
  const typename TInputImageType::SizeType&   inputSize
    = inputPtr->GetLargestPossibleRegion().GetSize();
  const typename TInputImageType::IndexType&  inputStartIndex
    = inputPtr->GetLargestPossibleRegion().GetIndex();
  
  typename TOutputImageType::SizeType     outputSize;
  typename TOutputImageType::IndexType    outputStartIndex;
  
  //
  // Size of output FFT:C2C is the same as input
  // 

  outputSize[0] = inputSize[0];
  outputStartIndex[0] = inputStartIndex[0];

  for (i = 1; i < TOutputImageType::ImageDimension; i++)
    {
    outputSize[i] = inputSize[i];
    outputStartIndex[i] = inputStartIndex[i];
    }
  typename TOutputImageType::RegionType outputLargestPossibleRegion;
  outputLargestPossibleRegion.SetSize( outputSize );
  outputLargestPossibleRegion.SetIndex( outputStartIndex );
  
  outputPtr->SetLargestPossibleRegion( outputLargestPossibleRegion );
}

template <class TPixel, unsigned int NDimension, int NDirection>
void
FFTComplexToComplexImageFilter<TPixel,NDimension,NDirection>::
GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
  // get pointers to the input and output
  typename TInputImageType::Pointer  inputPtr  = 
    const_cast<TInputImageType *>(this->GetInput());
  inputPtr->SetRequestedRegionToLargestPossibleRegion();
}

}
#endif
