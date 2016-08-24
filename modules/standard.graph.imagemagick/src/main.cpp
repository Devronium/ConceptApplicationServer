//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"
#include "AnsiString.h"

#include <wand/MagickWand.h>
//-------------------------//
// Local variables         //
//-------------------------//
INVOKE_CALL InvokePtr = 0;
//-----------------------------------------------------//
#define IMAGICK_FREE_MEMORY(value)     \
    if (value) {                       \
        MagickRelinquishMemory(value); \
        value = 0;                     \
    }

#define RETURN_STRING2(str) \
    char *s2 = str;         \
    RETURN_STRING(s2);      \
    IMAGICK_FREE_MEMORY(s2);

//-----------------------------------------------------//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    InvokePtr = Invoke;

    MagickWandGenesis();
    DEFINE_ECONSTANT(UndefinedEvaluateOperator)
    DEFINE_ECONSTANT(AddEvaluateOperator)
    DEFINE_ECONSTANT(AndEvaluateOperator)
    DEFINE_ECONSTANT(DivideEvaluateOperator)
    DEFINE_ECONSTANT(LeftShiftEvaluateOperator)
    DEFINE_ECONSTANT(MaxEvaluateOperator)
    DEFINE_ECONSTANT(MinEvaluateOperator)
    DEFINE_ECONSTANT(MultiplyEvaluateOperator)
    DEFINE_ECONSTANT(OrEvaluateOperator)
    DEFINE_ECONSTANT(RightShiftEvaluateOperator)
    DEFINE_ECONSTANT(SetEvaluateOperator)
    DEFINE_ECONSTANT(SubtractEvaluateOperator)
    DEFINE_ECONSTANT(XorEvaluateOperator)
    DEFINE_ECONSTANT(PowEvaluateOperator)
    DEFINE_ECONSTANT(LogEvaluateOperator)
    DEFINE_ECONSTANT(ThresholdEvaluateOperator)
    DEFINE_ECONSTANT(ThresholdBlackEvaluateOperator)
    DEFINE_ECONSTANT(ThresholdWhiteEvaluateOperator)
    DEFINE_ECONSTANT(GaussianNoiseEvaluateOperator)
    DEFINE_ECONSTANT(ImpulseNoiseEvaluateOperator)
    DEFINE_ECONSTANT(LaplacianNoiseEvaluateOperator)
    DEFINE_ECONSTANT(MultiplicativeNoiseEvaluateOperator)
    DEFINE_ECONSTANT(PoissonNoiseEvaluateOperator)
    DEFINE_ECONSTANT(UniformNoiseEvaluateOperator)
    DEFINE_ECONSTANT(CosineEvaluateOperator)
    DEFINE_ECONSTANT(SineEvaluateOperator)
    DEFINE_ECONSTANT(AddModulusEvaluateOperator)
    DEFINE_ECONSTANT(MeanEvaluateOperator)
    DEFINE_ECONSTANT(AbsEvaluateOperator)
    DEFINE_ECONSTANT(ExponentialEvaluateOperator)

    DEFINE_ECONSTANT(UndefinedFunction)
    DEFINE_ECONSTANT(PolynomialFunction)
    DEFINE_ECONSTANT(SinusoidFunction)
    DEFINE_ECONSTANT(ArcsinFunction)
    DEFINE_ECONSTANT(ArctanFunction)

    DEFINE_ECONSTANT(ReadMode)
    DEFINE_ECONSTANT(WriteMode)
    DEFINE_ECONSTANT(IOMode)

    DEFINE_ECONSTANT(ReadMode)
    DEFINE_ECONSTANT(WriteMode)
    DEFINE_ECONSTANT(IOMode)

    DEFINE_ECONSTANT(UndefinedVirtualPixelMethod)
    DEFINE_ECONSTANT(BackgroundVirtualPixelMethod)
    //DEFINE_ECONSTANT(ConstantVirtualPixelMethod) // deprecated
    DEFINE_ECONSTANT(DitherVirtualPixelMethod)
    DEFINE_ECONSTANT(EdgeVirtualPixelMethod)
    DEFINE_ECONSTANT(MirrorVirtualPixelMethod)
    DEFINE_ECONSTANT(RandomVirtualPixelMethod)
    DEFINE_ECONSTANT(TileVirtualPixelMethod)
    DEFINE_ECONSTANT(TransparentVirtualPixelMethod)
    DEFINE_ECONSTANT(MaskVirtualPixelMethod)
    DEFINE_ECONSTANT(BlackVirtualPixelMethod)
    DEFINE_ECONSTANT(GrayVirtualPixelMethod)
    DEFINE_ECONSTANT(WhiteVirtualPixelMethod)
    DEFINE_ECONSTANT(HorizontalTileVirtualPixelMethod)
    DEFINE_ECONSTANT(VerticalTileVirtualPixelMethod)
    DEFINE_ECONSTANT(HorizontalTileEdgeVirtualPixelMethod)
    DEFINE_ECONSTANT(VerticalTileEdgeVirtualPixelMethod)
    DEFINE_ECONSTANT(CheckerTileVirtualPixelMethod)

    DEFINE_ECONSTANT(UndefinedCompliance)
    DEFINE_ECONSTANT(NoCompliance)
    DEFINE_ECONSTANT(SVGCompliance)
    DEFINE_ECONSTANT(X11Compliance)
    DEFINE_ECONSTANT(XPMCompliance)
    DEFINE_ECONSTANT(AllCompliance)

    DEFINE_ECONSTANT(UndefinedColorspace)
    DEFINE_ECONSTANT(RGBColorspace)
    DEFINE_ECONSTANT(GRAYColorspace)
    DEFINE_ECONSTANT(TransparentColorspace)
    DEFINE_ECONSTANT(OHTAColorspace)
    DEFINE_ECONSTANT(LabColorspace)
    DEFINE_ECONSTANT(XYZColorspace)
    DEFINE_ECONSTANT(YCbCrColorspace)
    DEFINE_ECONSTANT(YCCColorspace)
    DEFINE_ECONSTANT(YIQColorspace)
    DEFINE_ECONSTANT(YPbPrColorspace)
    DEFINE_ECONSTANT(YUVColorspace)
    DEFINE_ECONSTANT(CMYKColorspace)
    DEFINE_ECONSTANT(sRGBColorspace)
    DEFINE_ECONSTANT(HSBColorspace)
    DEFINE_ECONSTANT(HSLColorspace)
    DEFINE_ECONSTANT(HWBColorspace)
    DEFINE_ECONSTANT(Rec601LumaColorspace)
    DEFINE_ECONSTANT(Rec601YCbCrColorspace)
    DEFINE_ECONSTANT(Rec709LumaColorspace)
    DEFINE_ECONSTANT(Rec709YCbCrColorspace)
    DEFINE_ECONSTANT(LogColorspace)
    DEFINE_ECONSTANT(CMYColorspace)

    DEFINE_ECONSTANT(UndefinedMetric)
    DEFINE_ECONSTANT(AbsoluteErrorMetric)
    DEFINE_ECONSTANT(MeanAbsoluteErrorMetric)
    DEFINE_ECONSTANT(MeanErrorPerPixelMetric)
    DEFINE_ECONSTANT(MeanSquaredErrorMetric)
    DEFINE_ECONSTANT(PeakAbsoluteErrorMetric)
    DEFINE_ECONSTANT(PeakSignalToNoiseRatioMetric)
    DEFINE_ECONSTANT(RootMeanSquaredErrorMetric)

    DEFINE_ECONSTANT(UndefinedCompositeOp)
    DEFINE_ECONSTANT(NoCompositeOp)
    DEFINE_ECONSTANT(ModulusAddCompositeOp)
    DEFINE_ECONSTANT(AtopCompositeOp)
    DEFINE_ECONSTANT(BlendCompositeOp)
    DEFINE_ECONSTANT(BumpmapCompositeOp)
    DEFINE_ECONSTANT(ChangeMaskCompositeOp)
    DEFINE_ECONSTANT(ClearCompositeOp)
    DEFINE_ECONSTANT(ColorBurnCompositeOp)
    DEFINE_ECONSTANT(ColorDodgeCompositeOp)
    DEFINE_ECONSTANT(ColorizeCompositeOp)
    DEFINE_ECONSTANT(CopyBlackCompositeOp)
    DEFINE_ECONSTANT(CopyBlueCompositeOp)
    DEFINE_ECONSTANT(CopyCompositeOp)
    DEFINE_ECONSTANT(CopyCyanCompositeOp)
    DEFINE_ECONSTANT(CopyGreenCompositeOp)
    DEFINE_ECONSTANT(CopyMagentaCompositeOp)
    DEFINE_ECONSTANT(CopyOpacityCompositeOp)
    DEFINE_ECONSTANT(CopyRedCompositeOp)
    DEFINE_ECONSTANT(CopyYellowCompositeOp)
    DEFINE_ECONSTANT(DarkenCompositeOp)
    DEFINE_ECONSTANT(DstAtopCompositeOp)
    DEFINE_ECONSTANT(DstCompositeOp)
    DEFINE_ECONSTANT(DstInCompositeOp)
    DEFINE_ECONSTANT(DstOutCompositeOp)
    DEFINE_ECONSTANT(DstOverCompositeOp)
    DEFINE_ECONSTANT(DifferenceCompositeOp)
    DEFINE_ECONSTANT(DisplaceCompositeOp)
    DEFINE_ECONSTANT(DissolveCompositeOp)
    DEFINE_ECONSTANT(ExclusionCompositeOp)
    DEFINE_ECONSTANT(HardLightCompositeOp)
    DEFINE_ECONSTANT(HueCompositeOp)
    DEFINE_ECONSTANT(InCompositeOp)
    DEFINE_ECONSTANT(LightenCompositeOp)
    DEFINE_ECONSTANT(LinearLightCompositeOp)
    DEFINE_ECONSTANT(LuminizeCompositeOp)
    DEFINE_ECONSTANT(MinusCompositeOp)
    DEFINE_ECONSTANT(ModulateCompositeOp)
    DEFINE_ECONSTANT(MultiplyCompositeOp)
    DEFINE_ECONSTANT(OutCompositeOp)
    DEFINE_ECONSTANT(OverCompositeOp)
    DEFINE_ECONSTANT(OverlayCompositeOp)
    DEFINE_ECONSTANT(PlusCompositeOp)
    DEFINE_ECONSTANT(ReplaceCompositeOp)
    DEFINE_ECONSTANT(SaturateCompositeOp)
    DEFINE_ECONSTANT(ScreenCompositeOp)
    DEFINE_ECONSTANT(SoftLightCompositeOp)
    DEFINE_ECONSTANT(SrcAtopCompositeOp)
    DEFINE_ECONSTANT(SrcCompositeOp)
    DEFINE_ECONSTANT(SrcInCompositeOp)
    DEFINE_ECONSTANT(SrcOutCompositeOp)
    DEFINE_ECONSTANT(SrcOverCompositeOp)
    DEFINE_ECONSTANT(ModulusSubtractCompositeOp)
    DEFINE_ECONSTANT(ThresholdCompositeOp)
    DEFINE_ECONSTANT(XorCompositeOp)
    DEFINE_ECONSTANT(DivideCompositeOp)
    DEFINE_ECONSTANT(DistortCompositeOp)
    DEFINE_ECONSTANT(BlurCompositeOp)
    DEFINE_ECONSTANT(PegtopLightCompositeOp)
    DEFINE_ECONSTANT(VividLightCompositeOp)
    DEFINE_ECONSTANT(PinLightCompositeOp)
    DEFINE_ECONSTANT(LinearDodgeCompositeOp)
    DEFINE_ECONSTANT(LinearBurnCompositeOp)
    DEFINE_ECONSTANT(MathematicsCompositeOp)
    DEFINE_ECONSTANT(UndefinedCompression)
    DEFINE_ECONSTANT(NoCompression)
    DEFINE_ECONSTANT(BZipCompression)
    DEFINE_ECONSTANT(DXT1Compression)
    DEFINE_ECONSTANT(DXT3Compression)
    DEFINE_ECONSTANT(DXT5Compression)
    DEFINE_ECONSTANT(FaxCompression)
    DEFINE_ECONSTANT(Group4Compression)
    DEFINE_ECONSTANT(JPEGCompression)
    DEFINE_ECONSTANT(JPEG2000Compression)
    DEFINE_ECONSTANT(LosslessJPEGCompression)
    DEFINE_ECONSTANT(LZWCompression)
    DEFINE_ECONSTANT(RLECompression)
    DEFINE_ECONSTANT(ZipCompression)
    DEFINE_ECONSTANT(ZipSCompression)
    DEFINE_ECONSTANT(PizCompression)
    DEFINE_ECONSTANT(Pxr24Compression)
    DEFINE_ECONSTANT(B44Compression)
    DEFINE_ECONSTANT(B44ACompression)
    DEFINE_ECONSTANT(UndefinedPixel)
    DEFINE_ECONSTANT(CharPixel)
    DEFINE_ECONSTANT(DoublePixel)
    DEFINE_ECONSTANT(FloatPixel)
    DEFINE_ECONSTANT(IntegerPixel)
    DEFINE_ECONSTANT(LongPixel)
    DEFINE_ECONSTANT(QuantumPixel)
    DEFINE_ECONSTANT(ShortPixel)
    DEFINE_ECONSTANT(UndefinedDistortion)
    DEFINE_ECONSTANT(AffineDistortion)
    DEFINE_ECONSTANT(AffineProjectionDistortion)
    DEFINE_ECONSTANT(ScaleRotateTranslateDistortion)
    DEFINE_ECONSTANT(PerspectiveDistortion)
    DEFINE_ECONSTANT(PerspectiveProjectionDistortion)
    DEFINE_ECONSTANT(BilinearForwardDistortion)
    DEFINE_ECONSTANT(BilinearDistortion)
    DEFINE_ECONSTANT(BilinearReverseDistortion)
    DEFINE_ECONSTANT(PolynomialDistortion)
    DEFINE_ECONSTANT(ArcDistortion)
    DEFINE_ECONSTANT(PolarDistortion)
    DEFINE_ECONSTANT(DePolarDistortion)
    DEFINE_ECONSTANT(BarrelDistortion)
    DEFINE_ECONSTANT(BarrelInverseDistortion)
    DEFINE_ECONSTANT(ShepardsDistortion)
    DEFINE_ECONSTANT(SentinelDistortion)
    DEFINE_ECONSTANT(UndefinedAlign)
    DEFINE_ECONSTANT(LeftAlign)
    DEFINE_ECONSTANT(CenterAlign)
    DEFINE_ECONSTANT(RightAlign)
    DEFINE_ECONSTANT(UndefinedPathUnits)
    DEFINE_ECONSTANT(UserSpace)
    DEFINE_ECONSTANT(UserSpaceOnUse)
    DEFINE_ECONSTANT(ObjectBoundingBox)
    DEFINE_ECONSTANT(UndefinedDecoration)
    DEFINE_ECONSTANT(NoDecoration)
    DEFINE_ECONSTANT(UnderlineDecoration)
    DEFINE_ECONSTANT(OverlineDecoration)
    DEFINE_ECONSTANT(LineThroughDecoration)
    DEFINE_ECONSTANT(UndefinedDecoration)
    DEFINE_ECONSTANT(NoDecoration)
    DEFINE_ECONSTANT(UnderlineDecoration)
    DEFINE_ECONSTANT(OverlineDecoration)
    DEFINE_ECONSTANT(LineThroughDecoration)
    DEFINE_ECONSTANT(UndefinedRule)
    DEFINE_ECONSTANT(EvenOddRule)
    DEFINE_ECONSTANT(NonZeroRule)
    DEFINE_ECONSTANT(UndefinedGradient)
    DEFINE_ECONSTANT(LinearGradient)
    DEFINE_ECONSTANT(RadialGradient)
    DEFINE_ECONSTANT(UndefinedCap)
    DEFINE_ECONSTANT(ButtCap)
    DEFINE_ECONSTANT(RoundCap)
    DEFINE_ECONSTANT(SquareCap)
    DEFINE_ECONSTANT(UndefinedJoin)
    DEFINE_ECONSTANT(MiterJoin)
    DEFINE_ECONSTANT(RoundJoin)
    DEFINE_ECONSTANT(BevelJoin)
    DEFINE_ECONSTANT(UndefinedMethod)
    DEFINE_ECONSTANT(PointMethod)
    DEFINE_ECONSTANT(ReplaceMethod)
    DEFINE_ECONSTANT(FloodfillMethod)
    DEFINE_ECONSTANT(FillToBorderMethod)
    DEFINE_ECONSTANT(ResetMethod)
    DEFINE_ECONSTANT(UndefinedPrimitive)
    DEFINE_ECONSTANT(PointPrimitive)
    DEFINE_ECONSTANT(LinePrimitive)
    DEFINE_ECONSTANT(RectanglePrimitive)
    DEFINE_ECONSTANT(RoundRectanglePrimitive)
    DEFINE_ECONSTANT(ArcPrimitive)
    DEFINE_ECONSTANT(EllipsePrimitive)
    DEFINE_ECONSTANT(CirclePrimitive)
    DEFINE_ECONSTANT(PolylinePrimitive)
    DEFINE_ECONSTANT(PolygonPrimitive)
    DEFINE_ECONSTANT(BezierPrimitive)
    DEFINE_ECONSTANT(ColorPrimitive)
    DEFINE_ECONSTANT(MattePrimitive)
    DEFINE_ECONSTANT(TextPrimitive)
    DEFINE_ECONSTANT(ImagePrimitive)
    DEFINE_ECONSTANT(PathPrimitive)
    DEFINE_ECONSTANT(UndefinedReference)
    DEFINE_ECONSTANT(GradientReference)
    DEFINE_ECONSTANT(UndefinedSpread)
    DEFINE_ECONSTANT(PadSpread)
    DEFINE_ECONSTANT(ReflectSpread)
    DEFINE_ECONSTANT(RepeatSpread)
    DEFINE_ECONSTANT(UndefinedPreview)
    DEFINE_ECONSTANT(RotatePreview)
    DEFINE_ECONSTANT(ShearPreview)
    DEFINE_ECONSTANT(RollPreview)
    DEFINE_ECONSTANT(HuePreview)
    DEFINE_ECONSTANT(SaturationPreview)
    DEFINE_ECONSTANT(BrightnessPreview)
    DEFINE_ECONSTANT(GammaPreview)
    DEFINE_ECONSTANT(SpiffPreview)
    DEFINE_ECONSTANT(DullPreview)
    DEFINE_ECONSTANT(GrayscalePreview)
    DEFINE_ECONSTANT(QuantizePreview)
    DEFINE_ECONSTANT(DespecklePreview)
    DEFINE_ECONSTANT(ReduceNoisePreview)
    DEFINE_ECONSTANT(AddNoisePreview)
    DEFINE_ECONSTANT(SharpenPreview)
    DEFINE_ECONSTANT(BlurPreview)
    DEFINE_ECONSTANT(ThresholdPreview)
    DEFINE_ECONSTANT(EdgeDetectPreview)
    DEFINE_ECONSTANT(SpreadPreview)
    DEFINE_ECONSTANT(SolarizePreview)
    DEFINE_ECONSTANT(ShadePreview)
    DEFINE_ECONSTANT(RaisePreview)
    DEFINE_ECONSTANT(SegmentPreview)
    DEFINE_ECONSTANT(SwirlPreview)
    DEFINE_ECONSTANT(ImplodePreview)
    DEFINE_ECONSTANT(WavePreview)
    DEFINE_ECONSTANT(OilPaintPreview)
    DEFINE_ECONSTANT(CharcoalDrawingPreview)
    DEFINE_ECONSTANT(JPEGPreview)
    DEFINE_ECONSTANT(UndefinedException)
    DEFINE_ECONSTANT(WarningException)
    DEFINE_ECONSTANT(ResourceLimitWarning)
    DEFINE_ECONSTANT(TypeWarning)
    DEFINE_ECONSTANT(OptionWarning)
    DEFINE_ECONSTANT(DelegateWarning)
    DEFINE_ECONSTANT(MissingDelegateWarning)
    DEFINE_ECONSTANT(CorruptImageWarning)
    DEFINE_ECONSTANT(FileOpenWarning)
    DEFINE_ECONSTANT(BlobWarning)
    DEFINE_ECONSTANT(StreamWarning)
    DEFINE_ECONSTANT(CacheWarning)
    DEFINE_ECONSTANT(CoderWarning)
    DEFINE_ECONSTANT(FilterWarning)
    DEFINE_ECONSTANT(ModuleWarning)
    DEFINE_ECONSTANT(DrawWarning)
    DEFINE_ECONSTANT(ImageWarning)
    DEFINE_ECONSTANT(WandWarning)
    DEFINE_ECONSTANT(RandomWarning)
    DEFINE_ECONSTANT(XServerWarning)
    DEFINE_ECONSTANT(MonitorWarning)
    DEFINE_ECONSTANT(RegistryWarning)
    DEFINE_ECONSTANT(ConfigureWarning)
    DEFINE_ECONSTANT(PolicyWarning)
    DEFINE_ECONSTANT(ErrorException)
    DEFINE_ECONSTANT(ResourceLimitError)
    DEFINE_ECONSTANT(TypeError)
    DEFINE_ECONSTANT(OptionError)
    DEFINE_ECONSTANT(DelegateError)
    DEFINE_ECONSTANT(MissingDelegateError)
    DEFINE_ECONSTANT(CorruptImageError)
    DEFINE_ECONSTANT(FileOpenError)
    DEFINE_ECONSTANT(BlobError)
    DEFINE_ECONSTANT(StreamError)
    DEFINE_ECONSTANT(CacheError)
    DEFINE_ECONSTANT(CoderError)
    DEFINE_ECONSTANT(FilterError)
    DEFINE_ECONSTANT(ModuleError)
    DEFINE_ECONSTANT(DrawError)
    DEFINE_ECONSTANT(ImageError)
    DEFINE_ECONSTANT(WandError)
    DEFINE_ECONSTANT(RandomError)
    DEFINE_ECONSTANT(XServerError)
    DEFINE_ECONSTANT(MonitorError)
    DEFINE_ECONSTANT(RegistryError)
    DEFINE_ECONSTANT(ConfigureError)
    DEFINE_ECONSTANT(PolicyError)
    DEFINE_ECONSTANT(FatalErrorException)
    DEFINE_ECONSTANT(ResourceLimitFatalError)
    DEFINE_ECONSTANT(TypeFatalError)
    DEFINE_ECONSTANT(OptionFatalError)
    DEFINE_ECONSTANT(DelegateFatalError)
    DEFINE_ECONSTANT(MissingDelegateFatalError)
    DEFINE_ECONSTANT(CorruptImageFatalError)
    DEFINE_ECONSTANT(FileOpenFatalError)
    DEFINE_ECONSTANT(BlobFatalError)
    DEFINE_ECONSTANT(StreamFatalError)
    DEFINE_ECONSTANT(CacheFatalError)
    DEFINE_ECONSTANT(CoderFatalError)
    DEFINE_ECONSTANT(FilterFatalError)
    DEFINE_ECONSTANT(ModuleFatalError)
    DEFINE_ECONSTANT(DrawFatalError)
    DEFINE_ECONSTANT(ImageFatalError)
    DEFINE_ECONSTANT(WandFatalError)
    DEFINE_ECONSTANT(RandomFatalError)
    DEFINE_ECONSTANT(XServerFatalError)
    DEFINE_ECONSTANT(MonitorFatalError)
    DEFINE_ECONSTANT(RegistryFatalError)
    DEFINE_ECONSTANT(ConfigureFatalError)
    DEFINE_ECONSTANT(PolicyFatalError)
    DEFINE_ECONSTANT(UndefinedNoise)
    DEFINE_ECONSTANT(UniformNoise)
    DEFINE_ECONSTANT(GaussianNoise)
    DEFINE_ECONSTANT(MultiplicativeGaussianNoise)
    DEFINE_ECONSTANT(ImpulseNoise)
    DEFINE_ECONSTANT(LaplacianNoise)
    DEFINE_ECONSTANT(PoissonNoise)
    DEFINE_ECONSTANT(RandomNoise)
    DEFINE_ECONSTANT(NoValue)
    DEFINE_ECONSTANT(XValue)
    DEFINE_ECONSTANT(XiValue)
    DEFINE_ECONSTANT(YValue)
    DEFINE_ECONSTANT(PsiValue)
    DEFINE_ECONSTANT(WidthValue)
    DEFINE_ECONSTANT(RhoValue)
    DEFINE_ECONSTANT(HeightValue)
    DEFINE_ECONSTANT(SigmaValue)
    DEFINE_ECONSTANT(ChiValue)
    DEFINE_ECONSTANT(XiNegative)
    DEFINE_ECONSTANT(XNegative)
    DEFINE_ECONSTANT(PsiNegative)
    DEFINE_ECONSTANT(YNegative)
    DEFINE_ECONSTANT(ChiNegative)
    DEFINE_ECONSTANT(PercentValue)
    DEFINE_ECONSTANT(AspectValue)
    DEFINE_ECONSTANT(NormalizeValue)
    DEFINE_ECONSTANT(LessValue)
    DEFINE_ECONSTANT(GreaterValue)
    DEFINE_ECONSTANT(MinimumValue)
    DEFINE_ECONSTANT(CorrelateNormalizeValue)
    DEFINE_ECONSTANT(AreaValue)
    DEFINE_ECONSTANT(DecimalValue)
    DEFINE_ECONSTANT(AllValues)
    DEFINE_ECONSTANT(UndefinedGravity)
    DEFINE_ECONSTANT(ForgetGravity)
    DEFINE_ECONSTANT(NorthWestGravity)
    DEFINE_ECONSTANT(NorthGravity)
    DEFINE_ECONSTANT(NorthEastGravity)
    DEFINE_ECONSTANT(WestGravity)
    DEFINE_ECONSTANT(CenterGravity)
    DEFINE_ECONSTANT(EastGravity)
    DEFINE_ECONSTANT(SouthWestGravity)
    DEFINE_ECONSTANT(SouthGravity)
    DEFINE_ECONSTANT(SouthEastGravity)
    DEFINE_ECONSTANT(StaticGravity)
    DEFINE_ECONSTANT(UndefinedAlphaChannel)
    DEFINE_ECONSTANT(ActivateAlphaChannel)
    DEFINE_ECONSTANT(BackgroundAlphaChannel)
    DEFINE_ECONSTANT(CopyAlphaChannel)
    DEFINE_ECONSTANT(DeactivateAlphaChannel)
    DEFINE_ECONSTANT(ExtractAlphaChannel)
    DEFINE_ECONSTANT(OpaqueAlphaChannel)
    DEFINE_ECONSTANT(ResetAlphaChannel)
    DEFINE_ECONSTANT(SetAlphaChannel)
    DEFINE_ECONSTANT(ShapeAlphaChannel)
    DEFINE_ECONSTANT(TransparentAlphaChannel)
    DEFINE_ECONSTANT(UndefinedType)
    DEFINE_ECONSTANT(BilevelType)
    DEFINE_ECONSTANT(GrayscaleType)
    DEFINE_ECONSTANT(GrayscaleMatteType)
    DEFINE_ECONSTANT(PaletteType)
    DEFINE_ECONSTANT(PaletteMatteType)
    DEFINE_ECONSTANT(TrueColorType)
    DEFINE_ECONSTANT(TrueColorMatteType)
    DEFINE_ECONSTANT(ColorSeparationType)
    DEFINE_ECONSTANT(ColorSeparationMatteType)
    DEFINE_ECONSTANT(OptimizeType)
    DEFINE_ECONSTANT(PaletteBilevelMatteType)
    DEFINE_ECONSTANT(UndefinedInterlace)
    DEFINE_ECONSTANT(NoInterlace)
    DEFINE_ECONSTANT(LineInterlace)
    DEFINE_ECONSTANT(PlaneInterlace)
    DEFINE_ECONSTANT(PartitionInterlace)
    DEFINE_ECONSTANT(GIFInterlace)
    DEFINE_ECONSTANT(JPEGInterlace)
    DEFINE_ECONSTANT(PNGInterlace)
    DEFINE_ECONSTANT(UndefinedOrientation)
    DEFINE_ECONSTANT(TopLeftOrientation)
    DEFINE_ECONSTANT(TopRightOrientation)
    DEFINE_ECONSTANT(BottomRightOrientation)
    DEFINE_ECONSTANT(BottomLeftOrientation)
    DEFINE_ECONSTANT(LeftTopOrientation)
    DEFINE_ECONSTANT(RightTopOrientation)
    DEFINE_ECONSTANT(RightBottomOrientation)
    DEFINE_ECONSTANT(LeftBottomOrientation)
    DEFINE_ECONSTANT(UndefinedResolution)
    DEFINE_ECONSTANT(PixelsPerInchResolution)
    DEFINE_ECONSTANT(PixelsPerCentimeterResolution)
    DEFINE_ECONSTANT(UnrecognizedDispose)
    DEFINE_ECONSTANT(UndefinedDispose)
    DEFINE_ECONSTANT(NoneDispose)
    DEFINE_ECONSTANT(BackgroundDispose)
    DEFINE_ECONSTANT(PreviousDispose)
    DEFINE_ECONSTANT(UndefinedLayer)
    DEFINE_ECONSTANT(CoalesceLayer)
    DEFINE_ECONSTANT(CompareAnyLayer)
    DEFINE_ECONSTANT(CompareClearLayer)
    DEFINE_ECONSTANT(CompareOverlayLayer)
    DEFINE_ECONSTANT(DisposeLayer)
    DEFINE_ECONSTANT(OptimizeLayer)
    DEFINE_ECONSTANT(OptimizeImageLayer)
    DEFINE_ECONSTANT(OptimizePlusLayer)
    DEFINE_ECONSTANT(OptimizeTransLayer)
    DEFINE_ECONSTANT(RemoveDupsLayer)
    DEFINE_ECONSTANT(RemoveZeroLayer)
    DEFINE_ECONSTANT(CompositeLayer)
    DEFINE_ECONSTANT(MergeLayer)
    DEFINE_ECONSTANT(FlattenLayer)
    DEFINE_ECONSTANT(MosaicLayer)
    DEFINE_ECONSTANT(TrimBoundsLayer)
    DEFINE_ECONSTANT(UndefinedEvents)
    DEFINE_ECONSTANT(NoEvents)
    DEFINE_ECONSTANT(TraceEvent)
    DEFINE_ECONSTANT(AnnotateEvent)
    DEFINE_ECONSTANT(BlobEvent)
    DEFINE_ECONSTANT(CacheEvent)
    DEFINE_ECONSTANT(CoderEvent)
    DEFINE_ECONSTANT(ConfigureEvent)
    DEFINE_ECONSTANT(DeprecateEvent)
    DEFINE_ECONSTANT(DrawEvent)
    DEFINE_ECONSTANT(ExceptionEvent)
    DEFINE_ECONSTANT(ImageEvent)
    DEFINE_ECONSTANT(LocaleEvent)
    DEFINE_ECONSTANT(ModuleEvent)
    DEFINE_ECONSTANT(PolicyEvent)
    DEFINE_ECONSTANT(ResourceEvent)
    DEFINE_ECONSTANT(TransformEvent)
    DEFINE_ECONSTANT(UserEvent)
    DEFINE_ECONSTANT(WandEvent)
    DEFINE_ECONSTANT(X11Event)
    DEFINE_ECONSTANT(AllEvents)
    DEFINE_ECONSTANT(UndefinedFormatType)
    DEFINE_ECONSTANT(ImplicitFormatType)
    DEFINE_ECONSTANT(ExplicitFormatType)
    DEFINE_ECONSTANT(NoThreadSupport)
    DEFINE_ECONSTANT(DecoderThreadSupport)
    DEFINE_ECONSTANT(EncoderThreadSupport)
    DEFINE_ECONSTANT(UndefinedChannel)
    DEFINE_ECONSTANT(RedChannel)
    DEFINE_ECONSTANT(GrayChannel)
    DEFINE_ECONSTANT(CyanChannel)
    DEFINE_ECONSTANT(GreenChannel)
    DEFINE_ECONSTANT(MagentaChannel)
    DEFINE_ECONSTANT(BlueChannel)
    DEFINE_ECONSTANT(YellowChannel)
    DEFINE_ECONSTANT(AlphaChannel)
    DEFINE_ECONSTANT(OpacityChannel)
    DEFINE_ECONSTANT(MatteChannel)
    DEFINE_ECONSTANT(BlackChannel)
    DEFINE_ECONSTANT(IndexChannel)
    DEFINE_ECONSTANT(AllChannels)
    DEFINE_ECONSTANT(TrueAlphaChannel)
    DEFINE_ECONSTANT(RGBChannels)
    DEFINE_ECONSTANT(GrayChannels)
    DEFINE_ECONSTANT(SyncChannels)
    DEFINE_ECONSTANT(DefaultChannels)
    DEFINE_ECONSTANT(UndefinedClass)
    DEFINE_ECONSTANT(DirectClass)
    DEFINE_ECONSTANT(PseudoClass)
    DEFINE_ECONSTANT(MagickFalse)
    DEFINE_ECONSTANT(MagickTrue)
    DEFINE_ECONSTANT(MagickImageCoderModule)
    DEFINE_ECONSTANT(MagickImageFilterModule)
    DEFINE_ECONSTANT(UndefinedMode)
    DEFINE_ECONSTANT(FrameMode)
    DEFINE_ECONSTANT(UnframeMode)
    DEFINE_ECONSTANT(ConcatenateMode)
    DEFINE_ECONSTANT(UndefinedKernel)
    DEFINE_ECONSTANT(UnityKernel)
    DEFINE_ECONSTANT(GaussianKernel)
    DEFINE_ECONSTANT(DoGKernel)
    DEFINE_ECONSTANT(LoGKernel)
    DEFINE_ECONSTANT(BlurKernel)
    DEFINE_ECONSTANT(CometKernel)
    DEFINE_ECONSTANT(LaplacianKernel)
    DEFINE_ECONSTANT(SobelKernel)
    DEFINE_ECONSTANT(FreiChenKernel)
    DEFINE_ECONSTANT(RobertsKernel)
    DEFINE_ECONSTANT(PrewittKernel)
    DEFINE_ECONSTANT(CompassKernel)
    DEFINE_ECONSTANT(KirschKernel)
    DEFINE_ECONSTANT(DiamondKernel)
    DEFINE_ECONSTANT(SquareKernel)
    DEFINE_ECONSTANT(RectangleKernel)
    DEFINE_ECONSTANT(DiskKernel)
    DEFINE_ECONSTANT(PlusKernel)
    DEFINE_ECONSTANT(CrossKernel)
    DEFINE_ECONSTANT(RingKernel)
    DEFINE_ECONSTANT(PeaksKernel)
    DEFINE_ECONSTANT(EdgesKernel)
    DEFINE_ECONSTANT(CornersKernel)
    DEFINE_ECONSTANT(ThinDiagonalsKernel)
    DEFINE_ECONSTANT(LineEndsKernel)
    DEFINE_ECONSTANT(LineJunctionsKernel)
    DEFINE_ECONSTANT(RidgesKernel)
    DEFINE_ECONSTANT(ConvexHullKernel)
    DEFINE_ECONSTANT(SkeletonKernel)
    DEFINE_ECONSTANT(ChebyshevKernel)
    DEFINE_ECONSTANT(ManhattanKernel)
    DEFINE_ECONSTANT(EuclideanKernel)
    DEFINE_ECONSTANT(UserDefinedKernel)
    DEFINE_ECONSTANT(UndefinedMorphology)
    DEFINE_ECONSTANT(ConvolveMorphology)
    DEFINE_ECONSTANT(CorrelateMorphology)
    DEFINE_ECONSTANT(ErodeMorphology)
    DEFINE_ECONSTANT(DilateMorphology)
    DEFINE_ECONSTANT(ErodeIntensityMorphology)
    DEFINE_ECONSTANT(DilateIntensityMorphology)
    DEFINE_ECONSTANT(DistanceMorphology)
    DEFINE_ECONSTANT(OpenMorphology)
    DEFINE_ECONSTANT(CloseMorphology)
    DEFINE_ECONSTANT(OpenIntensityMorphology)
    DEFINE_ECONSTANT(CloseIntensityMorphology)
    DEFINE_ECONSTANT(SmoothMorphology)
    DEFINE_ECONSTANT(EdgeInMorphology)
    DEFINE_ECONSTANT(EdgeOutMorphology)
    DEFINE_ECONSTANT(EdgeMorphology)
    DEFINE_ECONSTANT(TopHatMorphology)
    DEFINE_ECONSTANT(BottomHatMorphology)
    DEFINE_ECONSTANT(HitAndMissMorphology)
    DEFINE_ECONSTANT(ThinningMorphology)
    DEFINE_ECONSTANT(ThickenMorphology)
    DEFINE_ECONSTANT(MagickUndefinedOptions)
    DEFINE_ECONSTANT(MagickAlignOptions)
    DEFINE_ECONSTANT(MagickAlphaOptions)
    DEFINE_ECONSTANT(MagickBooleanOptions)
    DEFINE_ECONSTANT(MagickChannelOptions)
    DEFINE_ECONSTANT(MagickClassOptions)
    DEFINE_ECONSTANT(MagickClipPathOptions)
    DEFINE_ECONSTANT(MagickCoderOptions)
    DEFINE_ECONSTANT(MagickColorOptions)
    DEFINE_ECONSTANT(MagickColorspaceOptions)
    DEFINE_ECONSTANT(MagickCommandOptions)
    DEFINE_ECONSTANT(MagickComposeOptions)
    DEFINE_ECONSTANT(MagickCompressOptions)
    DEFINE_ECONSTANT(MagickConfigureOptions)
    DEFINE_ECONSTANT(MagickDataTypeOptions)
    DEFINE_ECONSTANT(MagickDebugOptions)
    DEFINE_ECONSTANT(MagickDecorateOptions)
    DEFINE_ECONSTANT(MagickDelegateOptions)
    DEFINE_ECONSTANT(MagickDirectionOptions)
    DEFINE_ECONSTANT(MagickDisposeOptions)
    DEFINE_ECONSTANT(MagickDistortOptions)
    DEFINE_ECONSTANT(MagickDitherOptions)
    DEFINE_ECONSTANT(MagickEndianOptions)
    DEFINE_ECONSTANT(MagickEvaluateOptions)
    DEFINE_ECONSTANT(MagickFillRuleOptions)
    DEFINE_ECONSTANT(MagickFilterOptions)
    DEFINE_ECONSTANT(MagickFontOptions)
    DEFINE_ECONSTANT(MagickFontsOptions)
    DEFINE_ECONSTANT(MagickFormatOptions)
    DEFINE_ECONSTANT(MagickFunctionOptions)
    DEFINE_ECONSTANT(MagickGravityOptions)
    DEFINE_ECONSTANT(MagickImageListOptions)
    DEFINE_ECONSTANT(MagickIntentOptions)
    DEFINE_ECONSTANT(MagickInterlaceOptions)
    DEFINE_ECONSTANT(MagickInterpolateOptions)
    DEFINE_ECONSTANT(MagickKernelOptions)
    DEFINE_ECONSTANT(MagickLayerOptions)
    DEFINE_ECONSTANT(MagickLineCapOptions)
    DEFINE_ECONSTANT(MagickLineJoinOptions)
    DEFINE_ECONSTANT(MagickListOptions)
    DEFINE_ECONSTANT(MagickLocaleOptions)
    DEFINE_ECONSTANT(MagickLogEventOptions)
    DEFINE_ECONSTANT(MagickLogOptions)
    DEFINE_ECONSTANT(MagickMagicOptions)
    DEFINE_ECONSTANT(MagickMethodOptions)
    DEFINE_ECONSTANT(MagickMetricOptions)
    DEFINE_ECONSTANT(MagickMimeOptions)
    DEFINE_ECONSTANT(MagickModeOptions)
    DEFINE_ECONSTANT(MagickModuleOptions)
    DEFINE_ECONSTANT(MagickMorphologyOptions)
    DEFINE_ECONSTANT(MagickNoiseOptions)
    DEFINE_ECONSTANT(MagickOrientationOptions)
    DEFINE_ECONSTANT(MagickPolicyOptions)
    DEFINE_ECONSTANT(MagickPolicyDomainOptions)
    DEFINE_ECONSTANT(MagickPolicyRightsOptions)
    DEFINE_ECONSTANT(MagickPreviewOptions)
    DEFINE_ECONSTANT(MagickPrimitiveOptions)
    DEFINE_ECONSTANT(MagickQuantumFormatOptions)
    DEFINE_ECONSTANT(MagickResolutionOptions)
    DEFINE_ECONSTANT(MagickResourceOptions)
    DEFINE_ECONSTANT(MagickSparseColorOptions)
    DEFINE_ECONSTANT(MagickStorageOptions)
    DEFINE_ECONSTANT(MagickStretchOptions)
    DEFINE_ECONSTANT(MagickStyleOptions)
    DEFINE_ECONSTANT(MagickThresholdOptions)
    DEFINE_ECONSTANT(MagickTypeOptions)
    DEFINE_ECONSTANT(MagickValidateOptions)
    DEFINE_ECONSTANT(MagickVirtualPixelOptions)
    DEFINE_ECONSTANT(UndefinedValidate)
    DEFINE_ECONSTANT(NoValidate)
    DEFINE_ECONSTANT(CompareValidate)
    DEFINE_ECONSTANT(CompositeValidate)
    DEFINE_ECONSTANT(ConvertValidate)
    DEFINE_ECONSTANT(FormatsInMemoryValidate)
    DEFINE_ECONSTANT(FormatsOnDiskValidate)
    DEFINE_ECONSTANT(IdentifyValidate)
    DEFINE_ECONSTANT(ImportExportValidate)
    DEFINE_ECONSTANT(MontageValidate)
    DEFINE_ECONSTANT(StreamValidate)
    DEFINE_ECONSTANT(AllValidate)
    DEFINE_ECONSTANT(UndefinedPolicyDomain)
    DEFINE_ECONSTANT(CoderPolicyDomain)
    DEFINE_ECONSTANT(DelegatePolicyDomain)
    DEFINE_ECONSTANT(FilterPolicyDomain)
    DEFINE_ECONSTANT(PathPolicyDomain)
    DEFINE_ECONSTANT(ResourcePolicyDomain)
    DEFINE_ECONSTANT(SystemPolicyDomain)
    DEFINE_ECONSTANT(UndefinedPolicyRights)
    DEFINE_ECONSTANT(NoPolicyRights)
    DEFINE_ECONSTANT(ReadPolicyRights)
    DEFINE_ECONSTANT(WritePolicyRights)
    DEFINE_ECONSTANT(ExecutePolicyRights)
    DEFINE_ECONSTANT(UndefinedIntent)
    DEFINE_ECONSTANT(SaturationIntent)
    DEFINE_ECONSTANT(PerceptualIntent)
    DEFINE_ECONSTANT(AbsoluteIntent)
    DEFINE_ECONSTANT(RelativeIntent)
    DEFINE_ECONSTANT(UndefinedDitherMethod)
    DEFINE_ECONSTANT(NoDitherMethod)
    DEFINE_ECONSTANT(RiemersmaDitherMethod)
    DEFINE_ECONSTANT(FloydSteinbergDitherMethod)
    DEFINE_ECONSTANT(UndefinedEndian)
    DEFINE_ECONSTANT(LSBEndian)
    DEFINE_ECONSTANT(MSBEndian)
    DEFINE_ECONSTANT(UndefinedQuantumAlpha)
    DEFINE_ECONSTANT(AssociatedQuantumAlpha)
    DEFINE_ECONSTANT(DisassociatedQuantumAlpha)
    DEFINE_ECONSTANT(UndefinedQuantumFormat)
    DEFINE_ECONSTANT(FloatingPointQuantumFormat)
    DEFINE_ECONSTANT(SignedQuantumFormat)
    DEFINE_ECONSTANT(UnsignedQuantumFormat)
    DEFINE_ECONSTANT(UndefinedQuantum)
    DEFINE_ECONSTANT(AlphaQuantum)
    DEFINE_ECONSTANT(BlackQuantum)
    DEFINE_ECONSTANT(BlueQuantum)
    DEFINE_ECONSTANT(CMYKAQuantum)
    DEFINE_ECONSTANT(CMYKQuantum)
    DEFINE_ECONSTANT(CyanQuantum)
    DEFINE_ECONSTANT(GrayAlphaQuantum)
    DEFINE_ECONSTANT(GrayQuantum)
    DEFINE_ECONSTANT(GreenQuantum)
    DEFINE_ECONSTANT(IndexAlphaQuantum)
    DEFINE_ECONSTANT(IndexQuantum)
    DEFINE_ECONSTANT(MagentaQuantum)
    DEFINE_ECONSTANT(OpacityQuantum)
    DEFINE_ECONSTANT(RedQuantum)
    DEFINE_ECONSTANT(RGBAQuantum)
    DEFINE_ECONSTANT(BGRAQuantum)
    DEFINE_ECONSTANT(RGBOQuantum)
    DEFINE_ECONSTANT(RGBQuantum)
    DEFINE_ECONSTANT(YellowQuantum)
    DEFINE_ECONSTANT(GrayPadQuantum)
    DEFINE_ECONSTANT(RGBPadQuantum)
    DEFINE_ECONSTANT(CbYCrYQuantum)
    DEFINE_ECONSTANT(CbYCrQuantum)
    DEFINE_ECONSTANT(CbYCrAQuantum)
    DEFINE_ECONSTANT(CMYKOQuantum)
    DEFINE_ECONSTANT(BGRQuantum)
    DEFINE_ECONSTANT(BGROQuantum)
    DEFINE_ECONSTANT(UndefinedRegistryType)
    DEFINE_ECONSTANT(ImageRegistryType)
    DEFINE_ECONSTANT(ImageInfoRegistryType)
    DEFINE_ECONSTANT(StringRegistryType)
    DEFINE_ECONSTANT(UndefinedFilter)
    DEFINE_ECONSTANT(PointFilter)
    DEFINE_ECONSTANT(BoxFilter)
    DEFINE_ECONSTANT(TriangleFilter)
    DEFINE_ECONSTANT(HermiteFilter)
    DEFINE_ECONSTANT(HanningFilter)
    DEFINE_ECONSTANT(HammingFilter)
    DEFINE_ECONSTANT(BlackmanFilter)
    DEFINE_ECONSTANT(GaussianFilter)
    DEFINE_ECONSTANT(QuadraticFilter)
    DEFINE_ECONSTANT(CubicFilter)
    DEFINE_ECONSTANT(CatromFilter)
    DEFINE_ECONSTANT(MitchellFilter)
    DEFINE_ECONSTANT(JincFilter)
    DEFINE_ECONSTANT(SincFilter)
    DEFINE_ECONSTANT(SincFastFilter)
    DEFINE_ECONSTANT(KaiserFilter)
    DEFINE_ECONSTANT(WelshFilter)
    DEFINE_ECONSTANT(ParzenFilter)
    DEFINE_ECONSTANT(BohmanFilter)
    DEFINE_ECONSTANT(BartlettFilter)
    DEFINE_ECONSTANT(LagrangeFilter)
    DEFINE_ECONSTANT(LanczosFilter)
    DEFINE_ECONSTANT(LanczosSharpFilter)
    DEFINE_ECONSTANT(Lanczos2Filter)
    DEFINE_ECONSTANT(Lanczos2SharpFilter)
    DEFINE_ECONSTANT(RobidouxFilter)
    DEFINE_ECONSTANT(SentinelFilter)
    DEFINE_ECONSTANT(UndefinedInterpolatePixel)
    DEFINE_ECONSTANT(AverageInterpolatePixel)
    DEFINE_ECONSTANT(BicubicInterpolatePixel)
    DEFINE_ECONSTANT(BilinearInterpolatePixel)
    DEFINE_ECONSTANT(FilterInterpolatePixel)
    DEFINE_ECONSTANT(IntegerInterpolatePixel)
    DEFINE_ECONSTANT(MeshInterpolatePixel)
    DEFINE_ECONSTANT(NearestNeighborInterpolatePixel)
    DEFINE_ECONSTANT(SplineInterpolatePixel)
    DEFINE_ECONSTANT(UndefinedResource)
    DEFINE_ECONSTANT(AreaResource)
    DEFINE_ECONSTANT(DiskResource)
    DEFINE_ECONSTANT(FileResource)
    DEFINE_ECONSTANT(MapResource)
    DEFINE_ECONSTANT(MemoryResource)
    DEFINE_ECONSTANT(ThreadResource)
    DEFINE_ECONSTANT(TimeResource)
    DEFINE_ECONSTANT(ThrottleResource)
    DEFINE_ECONSTANT(UndefinedTimerState)
    DEFINE_ECONSTANT(StoppedTimerState)
    DEFINE_ECONSTANT(RunningTimerState)
    DEFINE_ECONSTANT(UndefinedStretch)
    DEFINE_ECONSTANT(NormalStretch)
    DEFINE_ECONSTANT(UltraCondensedStretch)
    DEFINE_ECONSTANT(ExtraCondensedStretch)
    DEFINE_ECONSTANT(CondensedStretch)
    DEFINE_ECONSTANT(SemiCondensedStretch)
    DEFINE_ECONSTANT(SemiExpandedStretch)
    DEFINE_ECONSTANT(ExpandedStretch)
    DEFINE_ECONSTANT(ExtraExpandedStretch)
    DEFINE_ECONSTANT(UltraExpandedStretch)
    DEFINE_ECONSTANT(AnyStretch)
    DEFINE_ECONSTANT(UndefinedStyle)
    DEFINE_ECONSTANT(NormalStyle)
    DEFINE_ECONSTANT(ItalicStyle)
    DEFINE_ECONSTANT(ObliqueStyle)
    DEFINE_ECONSTANT(AnyStyle)
    DEFINE_ECONSTANT(UndefinedPath)
    DEFINE_ECONSTANT(MagickPath)
    DEFINE_ECONSTANT(RootPath)
    DEFINE_ECONSTANT(HeadPath)
    DEFINE_ECONSTANT(TailPath)
    DEFINE_ECONSTANT(BasePath)
    DEFINE_ECONSTANT(ExtensionPath)
    DEFINE_ECONSTANT(SubimagePath)
    DEFINE_ECONSTANT(CanonicalPath)
    return 0;
}
//-----------------------------------------------------//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    MagickWandTerminus();
    return 0;
}
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MagickRelinquishMemory, 1)
    T_NUMBER(MagickRelinquishMemory, 0)

    if (PARAM_INT(0)) {
        RETURN_NUMBER((SYS_INT)MagickRelinquishMemory((void *)PARAM_INT(0)))
    } else {
        RETURN_NUMBER(0)
    }
    SET_NUMBER(0, 0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MagickGetException, 1, 2)
    T_HANDLE(MagickGetException, 0)

    if (PARAMETERS_COUNT > 1) {
        SET_NUMBER(1, 0);
    }

    ExceptionType et;
    char          *str = MagickGetException((const MagickWand *)PARAM_INT(0), &et);
    if (str) {
        RETURN_STRING(str);
        MagickRelinquishMemory(str);
    } else {
        RETURN_STRING("");
    }
    if (PARAMETERS_COUNT > 1) {
        SET_NUMBER(1, (NUMBER)(INTEGER)et)
    }
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NewMagickWand, 0)
    RETURN_NUMBER((SYS_INT)NewMagickWand());
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MagickResizeImage, 4, 5)
    T_HANDLE(MagickResizeImage, 0)
    T_NUMBER(MagickResizeImage, 1)
    T_NUMBER(MagickResizeImage, 2)
    T_NUMBER(MagickResizeImage, 3)

    float blur = 1;
    if (PARAMETERS_COUNT > 3) {
        T_NUMBER(MagickResizeImage, 4)
        blur = PARAM(4);
    }
    RETURN_NUMBER(MagickResizeImage((MagickWand *)PARAM_INT(0), PARAM_INT(1), PARAM_INT(2), (FilterTypes)PARAM_INT(3), blur))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(MagickWriteImages, 1, 3)
    T_HANDLE(MagickWriteImages, 0)
    char *filename = 0;
    MagickBooleanType join_images = MagickFalse;
    if (PARAMETERS_COUNT > 1) {
        T_STRING(MagickWriteImages, 1)
        filename = PARAM(1);
    }
    if (PARAMETERS_COUNT > 2) {
        T_NUMBER(MagickWriteImages, 2)
        join_images = (MagickBooleanType)PARAM_INT(2);
    }

    RETURN_NUMBER(MagickWriteImages((MagickWand *)PARAM_INT(0), filename, join_images));
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MagickResetIterator, 1)
    T_HANDLE(MagickResetIterator, 0)
    MagickResetIterator((MagickWand *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MagickSetIteratorIndex, 2)
    T_HANDLE(MagickSetIteratorIndex, 0)
    T_NUMBER(MagickSetIteratorIndex, 1)
    RETURN_NUMBER(MagickSetIteratorIndex((MagickWand *)PARAM_INT(0), PARAM_INT(1)))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MagickGetIteratorIndex, 1)
    T_HANDLE(MagickGetIteratorIndex, 0)
    RETURN_NUMBER(MagickGetIteratorIndex((MagickWand *)PARAM_INT(0)))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MagickSetFirstIterator, 1)
    T_HANDLE(MagickSetFirstIterator, 0)
    MagickSetFirstIterator((MagickWand *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MagickSetLastIterator, 1)
    T_HANDLE(MagickSetLastIterator, 0)
    MagickSetLastIterator((MagickWand *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(DestroyMagickWand, 1)
    T_HANDLE(DestroyMagickWand, 0)

    RETURN_NUMBER((SYS_INT)DestroyMagickWand((MagickWand *)PARAM_INT(0)))
    SET_NUMBER(0, 0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(ClearMagickWand, 1)
    T_HANDLE(ClearMagickWand, 0)
    ClearMagickWand((MagickWand *)PARAM_INT(0));
    RETURN_NUMBER(0)
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(NewMagickWandFromImage, 1)
    T_HANDLE(NewMagickWandFromImage, 0)
    RETURN_NUMBER((SYS_INT)NewMagickWandFromImage((Image *)PARAM_INT(0)))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(CloneMagickWand, 1)
    T_HANDLE(CloneMagickWand, 0)
    RETURN_NUMBER((SYS_INT)CloneMagickWand((MagickWand *)PARAM_INT(0)))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(IsMagickWand, 1)
    T_HANDLE(IsMagickWand, 0)
    RETURN_NUMBER((SYS_INT)IsMagickWand((MagickWand *)PARAM_INT(0)))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MagickGetExceptionType, 1)
    T_HANDLE(MagickGetExceptionType, 0)
    RETURN_NUMBER((SYS_INT)MagickGetExceptionType((MagickWand *)PARAM_INT(0)))
END_IMPL
//-----------------------------------------------------//
CONCEPT_FUNCTION_IMPL(MagickClearException, 1)
    T_HANDLE(MagickClearException, 0)
    RETURN_NUMBER((SYS_INT)MagickClearException((MagickWand *)PARAM_INT(0)))
END_IMPL
//-----------------------------------------------------//
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAdaptiveBlurImage, 3)
    T_HANDLE(MagickAdaptiveBlurImage, 0)     // MagickWand*
    T_NUMBER(MagickAdaptiveBlurImage, 1)     // double
    T_NUMBER(MagickAdaptiveBlurImage, 2)     // double

    RETURN_NUMBER(MagickAdaptiveBlurImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAdaptiveBlurImageChannel, 4)
    T_HANDLE(MagickAdaptiveBlurImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickAdaptiveBlurImageChannel, 1)     // ChannelType
    T_NUMBER(MagickAdaptiveBlurImageChannel, 2)     // double
    T_NUMBER(MagickAdaptiveBlurImageChannel, 3)     // double

    RETURN_NUMBER(MagickAdaptiveBlurImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAdaptiveResizeImage, 3)
    T_HANDLE(MagickAdaptiveResizeImage, 0)     // MagickWand*
    T_NUMBER(MagickAdaptiveResizeImage, 1)     // size_t
    T_NUMBER(MagickAdaptiveResizeImage, 2)     // size_t

    RETURN_NUMBER(MagickAdaptiveResizeImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAdaptiveSharpenImage, 3)
    T_HANDLE(MagickAdaptiveSharpenImage, 0)     // MagickWand*
    T_NUMBER(MagickAdaptiveSharpenImage, 1)     // double
    T_NUMBER(MagickAdaptiveSharpenImage, 2)     // double

    RETURN_NUMBER(MagickAdaptiveSharpenImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAdaptiveSharpenImageChannel, 4)
    T_HANDLE(MagickAdaptiveSharpenImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickAdaptiveSharpenImageChannel, 1)     // ChannelType
    T_NUMBER(MagickAdaptiveSharpenImageChannel, 2)     // double
    T_NUMBER(MagickAdaptiveSharpenImageChannel, 3)     // double

    RETURN_NUMBER(MagickAdaptiveSharpenImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAdaptiveThresholdImage, 4)
    T_HANDLE(MagickAdaptiveThresholdImage, 0)     // MagickWand*
    T_NUMBER(MagickAdaptiveThresholdImage, 1)     // size_t
    T_NUMBER(MagickAdaptiveThresholdImage, 2)     // size_t
    T_NUMBER(MagickAdaptiveThresholdImage, 3)     // ssize_t

    RETURN_NUMBER(MagickAdaptiveThresholdImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (ssize_t)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAddImage, 2)
    T_HANDLE(MagickAddImage, 0)     // MagickWand*
    T_HANDLE(MagickAddImage, 1)     // MagickWand*

    RETURN_NUMBER(MagickAddImage((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAddNoiseImage, 2)
    T_HANDLE(MagickAddNoiseImage, 0)     // MagickWand*
    T_NUMBER(MagickAddNoiseImage, 1)     // NoiseType

    RETURN_NUMBER(MagickAddNoiseImage((MagickWand *)(long)PARAM(0), (NoiseType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAddNoiseImageChannel, 3)
    T_HANDLE(MagickAddNoiseImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickAddNoiseImageChannel, 1)     // ChannelType
    T_NUMBER(MagickAddNoiseImageChannel, 2)     // NoiseType

    RETURN_NUMBER(MagickAddNoiseImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (NoiseType)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAffineTransformImage, 2)
    T_HANDLE(MagickAffineTransformImage, 0)     // MagickWand*
    T_HANDLE(MagickAffineTransformImage, 1)     // DrawingWand*

    RETURN_NUMBER(MagickAffineTransformImage((MagickWand *)(long)PARAM(0), (DrawingWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAnnotateImage, 6)
    T_HANDLE(MagickAnnotateImage, 0)     // MagickWand*
    T_HANDLE(MagickAnnotateImage, 1)     // DrawingWand*
    T_NUMBER(MagickAnnotateImage, 2)     // double
    T_NUMBER(MagickAnnotateImage, 3)     // double
    T_NUMBER(MagickAnnotateImage, 4)     // double
    T_STRING(MagickAnnotateImage, 5)     // char*

    RETURN_NUMBER(MagickAnnotateImage((MagickWand *)(long)PARAM(0), (DrawingWand *)(long)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4), (char *)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAnimateImages, 2)
    T_HANDLE(MagickAnimateImages, 0)     // MagickWand*
    T_STRING(MagickAnimateImages, 1)     // char*

    RETURN_NUMBER(MagickAnimateImages((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAutoGammaImage, 1)
    T_HANDLE(MagickAutoGammaImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickAutoGammaImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAutoGammaImageChannel, 2)
    T_HANDLE(MagickAutoGammaImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickAutoGammaImageChannel, 1)     // ChannelType

    RETURN_NUMBER(MagickAutoGammaImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAutoLevelImage, 1)
    T_HANDLE(MagickAutoLevelImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickAutoLevelImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAutoLevelImageChannel, 2)
    T_HANDLE(MagickAutoLevelImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickAutoLevelImageChannel, 1)     // ChannelType

    RETURN_NUMBER(MagickAutoLevelImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickBlackThresholdImage, 2)
    T_HANDLE(MagickBlackThresholdImage, 0)     // MagickWand*
    T_HANDLE(MagickBlackThresholdImage, 1)     // PixelWand*

    RETURN_NUMBER(MagickBlackThresholdImage((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickBlueShiftImage, 2)
    T_HANDLE(MagickBlueShiftImage, 0)     // MagickWand*
    T_NUMBER(MagickBlueShiftImage, 1)     // double

    RETURN_NUMBER(MagickBlueShiftImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickBlurImage, 3)
    T_HANDLE(MagickBlurImage, 0)     // MagickWand*
    T_NUMBER(MagickBlurImage, 1)     // double
    T_NUMBER(MagickBlurImage, 2)     // double

    RETURN_NUMBER(MagickBlurImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickBlurImageChannel, 4)
    T_HANDLE(MagickBlurImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickBlurImageChannel, 1)     // ChannelType
    T_NUMBER(MagickBlurImageChannel, 2)     // double
    T_NUMBER(MagickBlurImageChannel, 3)     // double

    RETURN_NUMBER(MagickBlurImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickBorderImage, 4)
    T_HANDLE(MagickBorderImage, 0)     // MagickWand*
    T_HANDLE(MagickBorderImage, 1)     // PixelWand*
    T_NUMBER(MagickBorderImage, 2)     // size_t
    T_NUMBER(MagickBorderImage, 3)     // size_t

    RETURN_NUMBER(MagickBorderImage((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1), (size_t)PARAM(2), (size_t)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickBrightnessContrastImage, 3)
    T_HANDLE(MagickBrightnessContrastImage, 0)     // MagickWand*
    T_NUMBER(MagickBrightnessContrastImage, 1)     // double
    T_NUMBER(MagickBrightnessContrastImage, 2)     // double

    RETURN_NUMBER(MagickBrightnessContrastImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickBrightnessContrastImageChannel, 4)
    T_HANDLE(MagickBrightnessContrastImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickBrightnessContrastImageChannel, 1)     // ChannelType
    T_NUMBER(MagickBrightnessContrastImageChannel, 2)     // double
    T_NUMBER(MagickBrightnessContrastImageChannel, 3)     // double

    RETURN_NUMBER(MagickBrightnessContrastImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickCharcoalImage, 3)
    T_HANDLE(MagickCharcoalImage, 0)     // MagickWand*
    T_NUMBER(MagickCharcoalImage, 1)     // double
    T_NUMBER(MagickCharcoalImage, 2)     // double

    RETURN_NUMBER(MagickCharcoalImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickChopImage, 5)
    T_HANDLE(MagickChopImage, 0)     // MagickWand*
    T_NUMBER(MagickChopImage, 1)     // size_t
    T_NUMBER(MagickChopImage, 2)     // size_t
    T_NUMBER(MagickChopImage, 3)     // ssize_t
    T_NUMBER(MagickChopImage, 4)     // ssize_t

    RETURN_NUMBER(MagickChopImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickClampImage, 1)
    T_HANDLE(MagickClampImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickClampImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickClampImageChannel, 2)
    T_HANDLE(MagickClampImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickClampImageChannel, 1)     // ChannelType

    RETURN_NUMBER(MagickClampImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickClipImage, 1)
    T_HANDLE(MagickClipImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickClipImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickClipImagePath, 3)
    T_HANDLE(MagickClipImagePath, 0)     // MagickWand*
    T_STRING(MagickClipImagePath, 1)     // char*
    T_NUMBER(MagickClipImagePath, 2)     // MagickBooleanType

    RETURN_NUMBER(MagickClipImagePath((MagickWand *)(long)PARAM(0), (char *)PARAM(1), (MagickBooleanType)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickClutImage, 2)
    T_HANDLE(MagickClutImage, 0)     // MagickWand*
    T_HANDLE(MagickClutImage, 1)     // MagickWand*

    RETURN_NUMBER(MagickClutImage((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickClutImageChannel, 3)
    T_HANDLE(MagickClutImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickClutImageChannel, 1)     // ChannelType
    T_HANDLE(MagickClutImageChannel, 2)     // MagickWand*

    RETURN_NUMBER(MagickClutImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (MagickWand *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickColorDecisionListImage, 2)
    T_HANDLE(MagickColorDecisionListImage, 0)     // MagickWand*
    T_STRING(MagickColorDecisionListImage, 1)     // char*

    RETURN_NUMBER(MagickColorDecisionListImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickColorizeImage, 3)
    T_HANDLE(MagickColorizeImage, 0)     // MagickWand*
    T_HANDLE(MagickColorizeImage, 1)     // PixelWand*
    T_HANDLE(MagickColorizeImage, 2)     // PixelWand*

    RETURN_NUMBER(MagickColorizeImage((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1), (PixelWand *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickColorMatrixImage, 2)
    T_HANDLE(MagickColorMatrixImage, 0)     // MagickWand*
    T_HANDLE(MagickColorMatrixImage, 1)     // KernelInfo*

    RETURN_NUMBER(MagickColorMatrixImage((MagickWand *)(long)PARAM(0), (KernelInfo *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickCommentImage, 2)
    T_HANDLE(MagickCommentImage, 0)     // MagickWand*
    T_STRING(MagickCommentImage, 1)     // char*

    RETURN_NUMBER(MagickCommentImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickCompositeImage, 5)
    T_HANDLE(MagickCompositeImage, 0)     // MagickWand*
    T_HANDLE(MagickCompositeImage, 1)     // MagickWand*
    T_NUMBER(MagickCompositeImage, 2)     // CompositeOperator
    T_NUMBER(MagickCompositeImage, 3)     // ssize_t
    T_NUMBER(MagickCompositeImage, 4)     // ssize_t

    RETURN_NUMBER(MagickCompositeImage((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1), (CompositeOperator)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickCompositeImageChannel, 6)
    T_HANDLE(MagickCompositeImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickCompositeImageChannel, 1)     // ChannelType
    T_HANDLE(MagickCompositeImageChannel, 2)     // MagickWand*
    T_NUMBER(MagickCompositeImageChannel, 3)     // CompositeOperator
    T_NUMBER(MagickCompositeImageChannel, 4)     // ssize_t
    T_NUMBER(MagickCompositeImageChannel, 5)     // ssize_t

    RETURN_NUMBER(MagickCompositeImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (MagickWand *)(long)PARAM(2), (CompositeOperator)PARAM(3), (ssize_t)PARAM(4), (ssize_t)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickConstituteImage, 6)
    T_HANDLE(MagickConstituteImage, 0)     // MagickWand*
    T_NUMBER(MagickConstituteImage, 1)     // size_t
    T_NUMBER(MagickConstituteImage, 2)     // size_t
    T_STRING(MagickConstituteImage, 3)     // char*
    T_NUMBER(MagickConstituteImage, 4)     // StorageType
    T_NUMBER(MagickConstituteImage, 5)     // void*

    RETURN_NUMBER(MagickConstituteImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (char *)PARAM(3), (StorageType)PARAM(4), (void *)(long)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickContrastImage, 2)
    T_HANDLE(MagickContrastImage, 0)     // MagickWand*
    T_NUMBER(MagickContrastImage, 1)     // MagickBooleanType

    RETURN_NUMBER(MagickContrastImage((MagickWand *)(long)PARAM(0), (MagickBooleanType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickContrastStretchImage, 3)
    T_HANDLE(MagickContrastStretchImage, 0)     // MagickWand*
    T_NUMBER(MagickContrastStretchImage, 1)     // double
    T_NUMBER(MagickContrastStretchImage, 2)     // double

    RETURN_NUMBER(MagickContrastStretchImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickContrastStretchImageChannel, 4)
    T_HANDLE(MagickContrastStretchImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickContrastStretchImageChannel, 1)     // ChannelType
    T_NUMBER(MagickContrastStretchImageChannel, 2)     // double
    T_NUMBER(MagickContrastStretchImageChannel, 3)     // double

    RETURN_NUMBER(MagickContrastStretchImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickConvolveImage, 3)
    T_HANDLE(MagickConvolveImage, 0)     // MagickWand*
    T_NUMBER(MagickConvolveImage, 1)     // size_t

// ... parameter 2 is by reference (double*)
    double local_parameter_2;

    RETURN_NUMBER(MagickConvolveImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickConvolveImageChannel, 4)
    T_HANDLE(MagickConvolveImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickConvolveImageChannel, 1)     // ChannelType
    T_NUMBER(MagickConvolveImageChannel, 2)     // size_t

// ... parameter 3 is by reference (double*)
    double local_parameter_3;

    RETURN_NUMBER(MagickConvolveImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (size_t)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickCropImage, 5)
    T_HANDLE(MagickCropImage, 0)     // MagickWand*
    T_NUMBER(MagickCropImage, 1)     // size_t
    T_NUMBER(MagickCropImage, 2)     // size_t
    T_NUMBER(MagickCropImage, 3)     // ssize_t
    T_NUMBER(MagickCropImage, 4)     // ssize_t

    RETURN_NUMBER(MagickCropImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickCycleColormapImage, 2)
    T_HANDLE(MagickCycleColormapImage, 0)     // MagickWand*
    T_NUMBER(MagickCycleColormapImage, 1)     // ssize_t

    RETURN_NUMBER(MagickCycleColormapImage((MagickWand *)(long)PARAM(0), (ssize_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDecipherImage, 2)
    T_HANDLE(MagickDecipherImage, 0)     // MagickWand*
    T_STRING(MagickDecipherImage, 1)     // char*

    RETURN_NUMBER(MagickDecipherImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDeskewImage, 2)
    T_HANDLE(MagickDeskewImage, 0)     // MagickWand*
    T_NUMBER(MagickDeskewImage, 1)     // double

    RETURN_NUMBER(MagickDeskewImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDespeckleImage, 1)
    T_HANDLE(MagickDespeckleImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickDespeckleImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDisplayImage, 2)
    T_HANDLE(MagickDisplayImage, 0)     // MagickWand*
    T_STRING(MagickDisplayImage, 1)     // char*

    RETURN_NUMBER(MagickDisplayImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDisplayImages, 2)
    T_HANDLE(MagickDisplayImages, 0)     // MagickWand*
    T_STRING(MagickDisplayImages, 1)     // char*

    RETURN_NUMBER(MagickDisplayImages((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDistortImage, 5)
    T_HANDLE(MagickDistortImage, 0)     // MagickWand*
    T_NUMBER(MagickDistortImage, 1)     // DistortImageMethod
    T_NUMBER(MagickDistortImage, 2)     // size_t
    T_NUMBER(MagickDistortImage, 4)     // MagickBooleanType

// ... parameter 3 is by reference (double*)
    double local_parameter_3;

    RETURN_NUMBER(MagickDistortImage((MagickWand *)(long)PARAM(0), (DistortImageMethod)PARAM(1), (size_t)PARAM(2), &local_parameter_3, (MagickBooleanType)PARAM(4)))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDrawImage, 2)
    T_HANDLE(MagickDrawImage, 0)     // MagickWand*
    T_HANDLE(MagickDrawImage, 1)     // DrawingWand*

    RETURN_NUMBER(MagickDrawImage((MagickWand *)(long)PARAM(0), (DrawingWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickEdgeImage, 2)
    T_HANDLE(MagickEdgeImage, 0)     // MagickWand*
    T_NUMBER(MagickEdgeImage, 1)     // double

    RETURN_NUMBER(MagickEdgeImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickEmbossImage, 3)
    T_HANDLE(MagickEmbossImage, 0)     // MagickWand*
    T_NUMBER(MagickEmbossImage, 1)     // double
    T_NUMBER(MagickEmbossImage, 2)     // double

    RETURN_NUMBER(MagickEmbossImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickEncipherImage, 2)
    T_HANDLE(MagickEncipherImage, 0)     // MagickWand*
    T_STRING(MagickEncipherImage, 1)     // char*

    RETURN_NUMBER(MagickEncipherImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickEnhanceImage, 1)
    T_HANDLE(MagickEnhanceImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickEnhanceImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickEqualizeImage, 1)
    T_HANDLE(MagickEqualizeImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickEqualizeImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickEqualizeImageChannel, 2)
    T_HANDLE(MagickEqualizeImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickEqualizeImageChannel, 1)     // ChannelType

    RETURN_NUMBER(MagickEqualizeImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickEvaluateImage, 3)
    T_HANDLE(MagickEvaluateImage, 0)     // MagickWand*
    T_NUMBER(MagickEvaluateImage, 1)     // MagickEvaluateOperator
    T_NUMBER(MagickEvaluateImage, 2)     // double

    RETURN_NUMBER(MagickEvaluateImage((MagickWand *)(long)PARAM(0), (MagickEvaluateOperator)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickEvaluateImageChannel, 4)
    T_HANDLE(MagickEvaluateImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickEvaluateImageChannel, 1)     // ChannelType
    T_NUMBER(MagickEvaluateImageChannel, 2)     // MagickEvaluateOperator
    T_NUMBER(MagickEvaluateImageChannel, 3)     // double

    RETURN_NUMBER(MagickEvaluateImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (MagickEvaluateOperator)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickExportImagePixels, 8)
    T_HANDLE(MagickExportImagePixels, 0)     // MagickWand*
    T_NUMBER(MagickExportImagePixels, 1)     // ssize_t
    T_NUMBER(MagickExportImagePixels, 2)     // ssize_t
    T_NUMBER(MagickExportImagePixels, 3)     // size_t
    T_NUMBER(MagickExportImagePixels, 4)     // size_t
    T_STRING(MagickExportImagePixels, 5)     // char*
    T_NUMBER(MagickExportImagePixels, 6)     // StorageType
    T_NUMBER(MagickExportImagePixels, 7)     // void*

    RETURN_NUMBER(MagickExportImagePixels((MagickWand *)(long)PARAM(0), (ssize_t)PARAM(1), (ssize_t)PARAM(2), (size_t)PARAM(3), (size_t)PARAM(4), (char *)PARAM(5), (StorageType)PARAM(6), (void *)(long)PARAM(7)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickExtentImage, 5)
    T_HANDLE(MagickExtentImage, 0)     // MagickWand*
    T_NUMBER(MagickExtentImage, 1)     // size_t
    T_NUMBER(MagickExtentImage, 2)     // size_t
    T_NUMBER(MagickExtentImage, 3)     // ssize_t
    T_NUMBER(MagickExtentImage, 4)     // ssize_t

    RETURN_NUMBER(MagickExtentImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickFilterImage, 2)
    T_HANDLE(MagickFilterImage, 0)     // MagickWand*
    T_HANDLE(MagickFilterImage, 1)     // KernelInfo*

    RETURN_NUMBER(MagickFilterImage((MagickWand *)(long)PARAM(0), (KernelInfo *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickFilterImageChannel, 3)
    T_HANDLE(MagickFilterImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickFilterImageChannel, 1)     // ChannelType
    T_HANDLE(MagickFilterImageChannel, 2)     // KernelInfo*

    RETURN_NUMBER(MagickFilterImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (KernelInfo *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickFlipImage, 1)
    T_HANDLE(MagickFlipImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickFlipImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickFloodfillPaintImage, 8)
    T_HANDLE(MagickFloodfillPaintImage, 0)     // MagickWand*
    T_NUMBER(MagickFloodfillPaintImage, 1)     // ChannelType
    T_HANDLE(MagickFloodfillPaintImage, 2)     // PixelWand*
    T_NUMBER(MagickFloodfillPaintImage, 3)     // double
    T_HANDLE(MagickFloodfillPaintImage, 4)     // PixelWand*
    T_NUMBER(MagickFloodfillPaintImage, 5)     // ssize_t
    T_NUMBER(MagickFloodfillPaintImage, 6)     // ssize_t
    T_NUMBER(MagickFloodfillPaintImage, 7)     // MagickBooleanType

    RETURN_NUMBER(MagickFloodfillPaintImage((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (PixelWand *)(long)PARAM(2), (double)PARAM(3), (PixelWand *)(long)PARAM(4), (ssize_t)PARAM(5), (ssize_t)PARAM(6), (MagickBooleanType)PARAM(7)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickFlopImage, 1)
    T_HANDLE(MagickFlopImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickFlopImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickForwardFourierTransformImage, 2)
    T_HANDLE(MagickForwardFourierTransformImage, 0)     // MagickWand*
    T_NUMBER(MagickForwardFourierTransformImage, 1)     // MagickBooleanType

    RETURN_NUMBER(MagickForwardFourierTransformImage((MagickWand *)(long)PARAM(0), (MagickBooleanType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickFrameImage, 6)
    T_HANDLE(MagickFrameImage, 0)     // MagickWand*
    T_HANDLE(MagickFrameImage, 1)     // PixelWand*
    T_NUMBER(MagickFrameImage, 2)     // size_t
    T_NUMBER(MagickFrameImage, 3)     // size_t
    T_NUMBER(MagickFrameImage, 4)     // ssize_t
    T_NUMBER(MagickFrameImage, 5)     // ssize_t

    RETURN_NUMBER(MagickFrameImage((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1), (size_t)PARAM(2), (size_t)PARAM(3), (ssize_t)PARAM(4), (ssize_t)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickFunctionImage, 4)
    T_HANDLE(MagickFunctionImage, 0)     // MagickWand*
    T_NUMBER(MagickFunctionImage, 1)     // MagickFunction
    T_NUMBER(MagickFunctionImage, 2)     // size_t

// ... parameter 3 is by reference (double*)
    double local_parameter_3;

    RETURN_NUMBER(MagickFunctionImage((MagickWand *)(long)PARAM(0), (MagickFunction)PARAM(1), (size_t)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickFunctionImageChannel, 5)
    T_HANDLE(MagickFunctionImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickFunctionImageChannel, 1)     // ChannelType
    T_NUMBER(MagickFunctionImageChannel, 2)     // MagickFunction
    T_NUMBER(MagickFunctionImageChannel, 3)     // size_t

// ... parameter 4 is by reference (double*)
    double local_parameter_4;

    RETURN_NUMBER(MagickFunctionImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (MagickFunction)PARAM(2), (size_t)PARAM(3), &local_parameter_4))
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGammaImage, 2)
    T_HANDLE(MagickGammaImage, 0)     // MagickWand*
    T_NUMBER(MagickGammaImage, 1)     // double

    RETURN_NUMBER(MagickGammaImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGammaImageChannel, 3)
    T_HANDLE(MagickGammaImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickGammaImageChannel, 1)     // ChannelType
    T_NUMBER(MagickGammaImageChannel, 2)     // double

    RETURN_NUMBER(MagickGammaImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGaussianBlurImage, 3)
    T_HANDLE(MagickGaussianBlurImage, 0)     // MagickWand*
    T_NUMBER(MagickGaussianBlurImage, 1)     // double
    T_NUMBER(MagickGaussianBlurImage, 2)     // double

    RETURN_NUMBER(MagickGaussianBlurImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGaussianBlurImageChannel, 4)
    T_HANDLE(MagickGaussianBlurImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickGaussianBlurImageChannel, 1)     // ChannelType
    T_NUMBER(MagickGaussianBlurImageChannel, 2)     // double
    T_NUMBER(MagickGaussianBlurImageChannel, 3)     // double

    RETURN_NUMBER(MagickGaussianBlurImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageAlphaChannel, 1)
    T_HANDLE(MagickGetImageAlphaChannel, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageAlphaChannel((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageBackgroundColor, 2)
    T_HANDLE(MagickGetImageBackgroundColor, 0)     // MagickWand*
    T_HANDLE(MagickGetImageBackgroundColor, 1)     // PixelWand*

    RETURN_NUMBER(MagickGetImageBackgroundColor((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageBluePrimary, 3)
    T_HANDLE(MagickGetImageBluePrimary, 0)     // MagickWand*

// ... parameter 1 is by reference (double*)
    double local_parameter_1;
// ... parameter 2 is by reference (double*)
    double local_parameter_2;

    RETURN_NUMBER(MagickGetImageBluePrimary((MagickWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageBorderColor, 2)
    T_HANDLE(MagickGetImageBorderColor, 0)     // MagickWand*
    T_HANDLE(MagickGetImageBorderColor, 1)     // PixelWand*

    RETURN_NUMBER(MagickGetImageBorderColor((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageChannelDistortion, 5)
    T_HANDLE(MagickGetImageChannelDistortion, 0)     // MagickWand*
    T_HANDLE(MagickGetImageChannelDistortion, 1)     // MagickWand*
    T_NUMBER(MagickGetImageChannelDistortion, 2)     // ChannelType
    T_NUMBER(MagickGetImageChannelDistortion, 3)     // MetricType

// ... parameter 4 is by reference (double*)
    double local_parameter_4;

    RETURN_NUMBER(MagickGetImageChannelDistortion((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1), (ChannelType)PARAM(2), (MetricType)PARAM(3), &local_parameter_4))
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageChannelKurtosis, 4)
    T_HANDLE(MagickGetImageChannelKurtosis, 0)     // MagickWand*
    T_NUMBER(MagickGetImageChannelKurtosis, 1)     // ChannelType

// ... parameter 2 is by reference (double*)
    double local_parameter_2;
// ... parameter 3 is by reference (double*)
    double local_parameter_3;

    RETURN_NUMBER(MagickGetImageChannelKurtosis((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), &local_parameter_2, &local_parameter_3))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageChannelMean, 4)
    T_HANDLE(MagickGetImageChannelMean, 0)     // MagickWand*
    T_NUMBER(MagickGetImageChannelMean, 1)     // ChannelType

// ... parameter 2 is by reference (double*)
    double local_parameter_2;
// ... parameter 3 is by reference (double*)
    double local_parameter_3;

    RETURN_NUMBER(MagickGetImageChannelMean((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), &local_parameter_2, &local_parameter_3))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageChannelRange, 4)
    T_HANDLE(MagickGetImageChannelRange, 0)     // MagickWand*
    T_NUMBER(MagickGetImageChannelRange, 1)     // ChannelType

// ... parameter 2 is by reference (double*)
    double local_parameter_2;
// ... parameter 3 is by reference (double*)
    double local_parameter_3;

    RETURN_NUMBER(MagickGetImageChannelRange((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), &local_parameter_2, &local_parameter_3))
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageColormapColor, 3)
    T_HANDLE(MagickGetImageColormapColor, 0)     // MagickWand*
    T_NUMBER(MagickGetImageColormapColor, 1)     // size_t
    T_HANDLE(MagickGetImageColormapColor, 2)     // PixelWand*

    RETURN_NUMBER(MagickGetImageColormapColor((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (PixelWand *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageDistortion, 4)
    T_HANDLE(MagickGetImageDistortion, 0)     // MagickWand*
    T_HANDLE(MagickGetImageDistortion, 1)     // MagickWand*
    T_NUMBER(MagickGetImageDistortion, 2)     // MetricType

// ... parameter 3 is by reference (double*)
    double local_parameter_3;

    RETURN_NUMBER(MagickGetImageDistortion((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1), (MetricType)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageGreenPrimary, 3)
    T_HANDLE(MagickGetImageGreenPrimary, 0)     // MagickWand*

// ... parameter 1 is by reference (double*)
    double local_parameter_1;
// ... parameter 2 is by reference (double*)
    double local_parameter_2;

    RETURN_NUMBER(MagickGetImageGreenPrimary((MagickWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageMatteColor, 2)
    T_HANDLE(MagickGetImageMatteColor, 0)     // MagickWand*
    T_HANDLE(MagickGetImageMatteColor, 1)     // PixelWand*

    RETURN_NUMBER(MagickGetImageMatteColor((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageLength, 2)
    T_HANDLE(MagickGetImageLength, 0)     // MagickWand*

// ... parameter 1 is by reference (MagickSizeType*)
    MagickSizeType local_parameter_1;

    RETURN_NUMBER(MagickGetImageLength((MagickWand *)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImagePage, 5)
    T_HANDLE(MagickGetImagePage, 0)     // MagickWand*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;
// ... parameter 2 is by reference (size_t*)
    size_t local_parameter_2;
// ... parameter 3 is by reference (ssize_t*)
    ssize_t local_parameter_3;
// ... parameter 4 is by reference (ssize_t*)
    ssize_t local_parameter_4;

    RETURN_NUMBER(MagickGetImagePage((MagickWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2, &local_parameter_3, &local_parameter_4))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImagePixelColor, 4)
    T_HANDLE(MagickGetImagePixelColor, 0)     // MagickWand*
    T_NUMBER(MagickGetImagePixelColor, 1)     // ssize_t
    T_NUMBER(MagickGetImagePixelColor, 2)     // ssize_t
    T_HANDLE(MagickGetImagePixelColor, 3)     // PixelWand*

    RETURN_NUMBER(MagickGetImagePixelColor((MagickWand *)(long)PARAM(0), (ssize_t)PARAM(1), (ssize_t)PARAM(2), (PixelWand *)(long)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageRange, 3)
    T_HANDLE(MagickGetImageRange, 0)     // MagickWand*

// ... parameter 1 is by reference (double*)
    double local_parameter_1;
// ... parameter 2 is by reference (double*)
    double local_parameter_2;

    RETURN_NUMBER(MagickGetImageRange((MagickWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageRedPrimary, 3)
    T_HANDLE(MagickGetImageRedPrimary, 0)     // MagickWand*

// ... parameter 1 is by reference (double*)
    double local_parameter_1;
// ... parameter 2 is by reference (double*)
    double local_parameter_2;

    RETURN_NUMBER(MagickGetImageRedPrimary((MagickWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageResolution, 3)
    T_HANDLE(MagickGetImageResolution, 0)     // MagickWand*

// ... parameter 1 is by reference (double*)
    double local_parameter_1;
// ... parameter 2 is by reference (double*)
    double local_parameter_2;

    RETURN_NUMBER(MagickGetImageResolution((MagickWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageWhitePoint, 3)
    T_HANDLE(MagickGetImageWhitePoint, 0)     // MagickWand*

// ... parameter 1 is by reference (double*)
    double local_parameter_1;
// ... parameter 2 is by reference (double*)
    double local_parameter_2;

    RETURN_NUMBER(MagickGetImageWhitePoint((MagickWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickHaldClutImage, 2)
    T_HANDLE(MagickHaldClutImage, 0)     // MagickWand*
    T_HANDLE(MagickHaldClutImage, 1)     // MagickWand*

    RETURN_NUMBER(MagickHaldClutImage((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickHaldClutImageChannel, 3)
    T_HANDLE(MagickHaldClutImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickHaldClutImageChannel, 1)     // ChannelType
    T_HANDLE(MagickHaldClutImageChannel, 2)     // MagickWand*

    RETURN_NUMBER(MagickHaldClutImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (MagickWand *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickHasNextImage, 1)
    T_HANDLE(MagickHasNextImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickHasNextImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickHasPreviousImage, 1)
    T_HANDLE(MagickHasPreviousImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickHasPreviousImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickImplodeImage, 2)
    T_HANDLE(MagickImplodeImage, 0)     // MagickWand*
    T_NUMBER(MagickImplodeImage, 1)     // double

    RETURN_NUMBER(MagickImplodeImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickImportImagePixels, 8)
    T_HANDLE(MagickImportImagePixels, 0)     // MagickWand*
    T_NUMBER(MagickImportImagePixels, 1)     // ssize_t
    T_NUMBER(MagickImportImagePixels, 2)     // ssize_t
    T_NUMBER(MagickImportImagePixels, 3)     // size_t
    T_NUMBER(MagickImportImagePixels, 4)     // size_t
    T_STRING(MagickImportImagePixels, 5)     // char*
    T_NUMBER(MagickImportImagePixels, 6)     // StorageType
    T_NUMBER(MagickImportImagePixels, 7)     // void*

    RETURN_NUMBER(MagickImportImagePixels((MagickWand *)(long)PARAM(0), (ssize_t)PARAM(1), (ssize_t)PARAM(2), (size_t)PARAM(3), (size_t)PARAM(4), (char *)PARAM(5), (StorageType)PARAM(6), (void *)(long)PARAM(7)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickInverseFourierTransformImage, 3)
    T_HANDLE(MagickInverseFourierTransformImage, 0)     // MagickWand*
    T_HANDLE(MagickInverseFourierTransformImage, 1)     // MagickWand*
    T_NUMBER(MagickInverseFourierTransformImage, 2)     // MagickBooleanType

    RETURN_NUMBER(MagickInverseFourierTransformImage((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1), (MagickBooleanType)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickLabelImage, 2)
    T_HANDLE(MagickLabelImage, 0)     // MagickWand*
    T_STRING(MagickLabelImage, 1)     // char*

    RETURN_NUMBER(MagickLabelImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickLevelImage, 4)
    T_HANDLE(MagickLevelImage, 0)     // MagickWand*
    T_NUMBER(MagickLevelImage, 1)     // double
    T_NUMBER(MagickLevelImage, 2)     // double
    T_NUMBER(MagickLevelImage, 3)     // double

    RETURN_NUMBER(MagickLevelImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickLevelImageChannel, 5)
    T_HANDLE(MagickLevelImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickLevelImageChannel, 1)     // ChannelType
    T_NUMBER(MagickLevelImageChannel, 2)     // double
    T_NUMBER(MagickLevelImageChannel, 3)     // double
    T_NUMBER(MagickLevelImageChannel, 4)     // double

    RETURN_NUMBER(MagickLevelImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickLinearStretchImage, 3)
    T_HANDLE(MagickLinearStretchImage, 0)     // MagickWand*
    T_NUMBER(MagickLinearStretchImage, 1)     // double
    T_NUMBER(MagickLinearStretchImage, 2)     // double

    RETURN_NUMBER(MagickLinearStretchImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickLiquidRescaleImage, 5)
    T_HANDLE(MagickLiquidRescaleImage, 0)     // MagickWand*
    T_NUMBER(MagickLiquidRescaleImage, 1)     // size_t
    T_NUMBER(MagickLiquidRescaleImage, 2)     // size_t
    T_NUMBER(MagickLiquidRescaleImage, 3)     // double
    T_NUMBER(MagickLiquidRescaleImage, 4)     // double

    RETURN_NUMBER(MagickLiquidRescaleImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (double)PARAM(3), (double)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickMagnifyImage, 1)
    T_HANDLE(MagickMagnifyImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickMagnifyImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickMedianFilterImage, 2)
    T_HANDLE(MagickMedianFilterImage, 0)     // MagickWand*
    T_NUMBER(MagickMedianFilterImage, 1)     // double

    RETURN_NUMBER(MagickMedianFilterImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickMinifyImage, 1)
    T_HANDLE(MagickMinifyImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickMinifyImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickModulateImage, 4)
    T_HANDLE(MagickModulateImage, 0)     // MagickWand*
    T_NUMBER(MagickModulateImage, 1)     // double
    T_NUMBER(MagickModulateImage, 2)     // double
    T_NUMBER(MagickModulateImage, 3)     // double

    RETURN_NUMBER(MagickModulateImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickMorphologyImage, 4)
    T_HANDLE(MagickMorphologyImage, 0)     // MagickWand*
    T_NUMBER(MagickMorphologyImage, 1)     // MorphologyMethod
    T_NUMBER(MagickMorphologyImage, 2)     // ssize_t
    T_HANDLE(MagickMorphologyImage, 3)     // KernelInfo*

    RETURN_NUMBER(MagickMorphologyImage((MagickWand *)(long)PARAM(0), (MorphologyMethod)PARAM(1), (ssize_t)PARAM(2), (KernelInfo *)(long)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickMorphologyImageChannel, 5)
    T_HANDLE(MagickMorphologyImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickMorphologyImageChannel, 1)     // ChannelType
    T_NUMBER(MagickMorphologyImageChannel, 2)     // MorphologyMethod
    T_NUMBER(MagickMorphologyImageChannel, 3)     // ssize_t
    T_HANDLE(MagickMorphologyImageChannel, 4)     // KernelInfo*

    RETURN_NUMBER(MagickMorphologyImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (MorphologyMethod)PARAM(2), (ssize_t)PARAM(3), (KernelInfo *)(long)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickMotionBlurImage, 4)
    T_HANDLE(MagickMotionBlurImage, 0)     // MagickWand*
    T_NUMBER(MagickMotionBlurImage, 1)     // double
    T_NUMBER(MagickMotionBlurImage, 2)     // double
    T_NUMBER(MagickMotionBlurImage, 3)     // double

    RETURN_NUMBER(MagickMotionBlurImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickMotionBlurImageChannel, 5)
    T_HANDLE(MagickMotionBlurImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickMotionBlurImageChannel, 1)     // ChannelType
    T_NUMBER(MagickMotionBlurImageChannel, 2)     // double
    T_NUMBER(MagickMotionBlurImageChannel, 3)     // double
    T_NUMBER(MagickMotionBlurImageChannel, 4)     // double

    RETURN_NUMBER(MagickMotionBlurImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickNegateImage, 2)
    T_HANDLE(MagickNegateImage, 0)     // MagickWand*
    T_NUMBER(MagickNegateImage, 1)     // MagickBooleanType

    RETURN_NUMBER(MagickNegateImage((MagickWand *)(long)PARAM(0), (MagickBooleanType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickNegateImageChannel, 3)
    T_HANDLE(MagickNegateImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickNegateImageChannel, 1)     // ChannelType
    T_NUMBER(MagickNegateImageChannel, 2)     // MagickBooleanType

    RETURN_NUMBER(MagickNegateImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (MagickBooleanType)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickNewImage, 4)
    T_HANDLE(MagickNewImage, 0)     // MagickWand*
    T_NUMBER(MagickNewImage, 1)     // size_t
    T_NUMBER(MagickNewImage, 2)     // size_t
    T_HANDLE(MagickNewImage, 3)     // PixelWand*

    RETURN_NUMBER(MagickNewImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (PixelWand *)(long)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickNextImage, 1)
    T_HANDLE(MagickNextImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickNextImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickNormalizeImage, 1)
    T_HANDLE(MagickNormalizeImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickNormalizeImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickNormalizeImageChannel, 2)
    T_HANDLE(MagickNormalizeImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickNormalizeImageChannel, 1)     // ChannelType

    RETURN_NUMBER(MagickNormalizeImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickOilPaintImage, 2)
    T_HANDLE(MagickOilPaintImage, 0)     // MagickWand*
    T_NUMBER(MagickOilPaintImage, 1)     // double

    RETURN_NUMBER(MagickOilPaintImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickOpaquePaintImage, 5)
    T_HANDLE(MagickOpaquePaintImage, 0)     // MagickWand*
    T_HANDLE(MagickOpaquePaintImage, 1)     // PixelWand*
    T_HANDLE(MagickOpaquePaintImage, 2)     // PixelWand*
    T_NUMBER(MagickOpaquePaintImage, 3)     // double
    T_NUMBER(MagickOpaquePaintImage, 4)     // MagickBooleanType

    RETURN_NUMBER(MagickOpaquePaintImage((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1), (PixelWand *)(long)PARAM(2), (double)PARAM(3), (MagickBooleanType)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickOpaquePaintImageChannel, 6)
    T_HANDLE(MagickOpaquePaintImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickOpaquePaintImageChannel, 1)     // ChannelType
    T_HANDLE(MagickOpaquePaintImageChannel, 2)     // PixelWand*
    T_HANDLE(MagickOpaquePaintImageChannel, 3)     // PixelWand*
    T_NUMBER(MagickOpaquePaintImageChannel, 4)     // double
    T_NUMBER(MagickOpaquePaintImageChannel, 5)     // MagickBooleanType

    RETURN_NUMBER(MagickOpaquePaintImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (PixelWand *)(long)PARAM(2), (PixelWand *)(long)PARAM(3), (double)PARAM(4), (MagickBooleanType)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickOrderedPosterizeImage, 2)
    T_HANDLE(MagickOrderedPosterizeImage, 0)     // MagickWand*
    T_STRING(MagickOrderedPosterizeImage, 1)     // char*

    RETURN_NUMBER(MagickOrderedPosterizeImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickOrderedPosterizeImageChannel, 3)
    T_HANDLE(MagickOrderedPosterizeImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickOrderedPosterizeImageChannel, 1)     // ChannelType
    T_STRING(MagickOrderedPosterizeImageChannel, 2)     // char*

    RETURN_NUMBER(MagickOrderedPosterizeImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickTransparentPaintImage, 5)
    T_HANDLE(MagickTransparentPaintImage, 0)     // MagickWand*
    T_HANDLE(MagickTransparentPaintImage, 1)     // PixelWand*
    T_NUMBER(MagickTransparentPaintImage, 2)     // double
    T_NUMBER(MagickTransparentPaintImage, 3)     // double
    T_NUMBER(MagickTransparentPaintImage, 4)     // MagickBooleanType

    RETURN_NUMBER(MagickTransparentPaintImage((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1), (double)PARAM(2), (double)PARAM(3), (MagickBooleanType)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickPingImage, 2)
    T_HANDLE(MagickPingImage, 0)     // MagickWand*
    T_STRING(MagickPingImage, 1)     // char*

    RETURN_NUMBER(MagickPingImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickPingImageBlob, 2)
    T_HANDLE(MagickPingImageBlob, 0)     // MagickWand*
    T_STRING(MagickPingImageBlob, 1)     // void*

    RETURN_NUMBER(MagickPingImageBlob((MagickWand *)(long)PARAM(0), (void *)PARAM(1), (size_t)PARAM_LEN(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickPingImageFile, 2)
    T_HANDLE(MagickPingImageFile, 0)     // MagickWand*
    T_HANDLE(MagickPingImageFile, 1)     // FILE*

    RETURN_NUMBER(MagickPingImageFile((MagickWand *)(long)PARAM(0), (FILE *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickPolaroidImage, 3)
    T_HANDLE(MagickPolaroidImage, 0)     // MagickWand*
    T_HANDLE(MagickPolaroidImage, 1)     // DrawingWand*
    T_NUMBER(MagickPolaroidImage, 2)     // double

    RETURN_NUMBER(MagickPolaroidImage((MagickWand *)(long)PARAM(0), (DrawingWand *)(long)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickPosterizeImage, 3)
    T_HANDLE(MagickPosterizeImage, 0)     // MagickWand*
    T_NUMBER(MagickPosterizeImage, 1)     // size_t
    T_NUMBER(MagickPosterizeImage, 2)     // MagickBooleanType

    RETURN_NUMBER(MagickPosterizeImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (MagickBooleanType)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickPreviousImage, 1)
    T_HANDLE(MagickPreviousImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickPreviousImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickQuantizeImage, 6)
    T_HANDLE(MagickQuantizeImage, 0)     // MagickWand*
    T_NUMBER(MagickQuantizeImage, 1)     // size_t
    T_NUMBER(MagickQuantizeImage, 2)     // ColorspaceType
    T_NUMBER(MagickQuantizeImage, 3)     // size_t
    T_NUMBER(MagickQuantizeImage, 4)     // MagickBooleanType
    T_NUMBER(MagickQuantizeImage, 5)     // MagickBooleanType

    RETURN_NUMBER(MagickQuantizeImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (ColorspaceType)PARAM(2), (size_t)PARAM(3), (MagickBooleanType)PARAM(4), (MagickBooleanType)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickQuantizeImages, 6)
    T_HANDLE(MagickQuantizeImages, 0)     // MagickWand*
    T_NUMBER(MagickQuantizeImages, 1)     // size_t
    T_NUMBER(MagickQuantizeImages, 2)     // ColorspaceType
    T_NUMBER(MagickQuantizeImages, 3)     // size_t
    T_NUMBER(MagickQuantizeImages, 4)     // MagickBooleanType
    T_NUMBER(MagickQuantizeImages, 5)     // MagickBooleanType

    RETURN_NUMBER(MagickQuantizeImages((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (ColorspaceType)PARAM(2), (size_t)PARAM(3), (MagickBooleanType)PARAM(4), (MagickBooleanType)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickRadialBlurImage, 2)
    T_HANDLE(MagickRadialBlurImage, 0)     // MagickWand*
    T_NUMBER(MagickRadialBlurImage, 1)     // double

    RETURN_NUMBER(MagickRadialBlurImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickRadialBlurImageChannel, 3)
    T_HANDLE(MagickRadialBlurImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickRadialBlurImageChannel, 1)     // ChannelType
    T_NUMBER(MagickRadialBlurImageChannel, 2)     // double

    RETURN_NUMBER(MagickRadialBlurImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickRaiseImage, 6)
    T_HANDLE(MagickRaiseImage, 0)     // MagickWand*
    T_NUMBER(MagickRaiseImage, 1)     // size_t
    T_NUMBER(MagickRaiseImage, 2)     // size_t
    T_NUMBER(MagickRaiseImage, 3)     // ssize_t
    T_NUMBER(MagickRaiseImage, 4)     // ssize_t
    T_NUMBER(MagickRaiseImage, 5)     // MagickBooleanType

    RETURN_NUMBER(MagickRaiseImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4), (MagickBooleanType)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickRandomThresholdImage, 3)
    T_HANDLE(MagickRandomThresholdImage, 0)     // MagickWand*
    T_NUMBER(MagickRandomThresholdImage, 1)     // double
    T_NUMBER(MagickRandomThresholdImage, 2)     // double

    RETURN_NUMBER(MagickRandomThresholdImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickRandomThresholdImageChannel, 4)
    T_HANDLE(MagickRandomThresholdImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickRandomThresholdImageChannel, 1)     // ChannelType
    T_NUMBER(MagickRandomThresholdImageChannel, 2)     // double
    T_NUMBER(MagickRandomThresholdImageChannel, 3)     // double

    RETURN_NUMBER(MagickRandomThresholdImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickReadImage, 2)
    T_HANDLE(MagickReadImage, 0)     // MagickWand*
    T_STRING(MagickReadImage, 1)     // char*

    RETURN_NUMBER(MagickReadImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickReadImageBlob, 2)
    T_HANDLE(MagickReadImageBlob, 0)     // MagickWand*
    T_STRING(MagickReadImageBlob, 1)     // void*

    RETURN_NUMBER(MagickReadImageBlob((MagickWand *)(long)PARAM(0), (void *)PARAM(1), (size_t)PARAM_LEN(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickReadImageFile, 2)
    T_HANDLE(MagickReadImageFile, 0)     // MagickWand*
    T_HANDLE(MagickReadImageFile, 1)     // FILE*

    RETURN_NUMBER(MagickReadImageFile((MagickWand *)(long)PARAM(0), (FILE *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickReduceNoiseImage, 2)
    T_HANDLE(MagickReduceNoiseImage, 0)     // MagickWand*
    T_NUMBER(MagickReduceNoiseImage, 1)     // double

    RETURN_NUMBER(MagickReduceNoiseImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickRemapImage, 3)
    T_HANDLE(MagickRemapImage, 0)     // MagickWand*
    T_HANDLE(MagickRemapImage, 1)     // MagickWand*
    T_NUMBER(MagickRemapImage, 2)     // DitherMethod

    RETURN_NUMBER(MagickRemapImage((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1), (DitherMethod)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickRemoveImage, 1)
    T_HANDLE(MagickRemoveImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickRemoveImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickResampleImage, 5)
    T_HANDLE(MagickResampleImage, 0)     // MagickWand*
    T_NUMBER(MagickResampleImage, 1)     // double
    T_NUMBER(MagickResampleImage, 2)     // double
    T_NUMBER(MagickResampleImage, 3)     // FilterTypes
    T_NUMBER(MagickResampleImage, 4)     // double

    RETURN_NUMBER(MagickResampleImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (FilterTypes)PARAM(3), (double)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickResetImagePage, 2)
    T_HANDLE(MagickResetImagePage, 0)     // MagickWand*
    T_STRING(MagickResetImagePage, 1)     // char*

    RETURN_NUMBER(MagickResetImagePage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickRollImage, 3)
    T_HANDLE(MagickRollImage, 0)     // MagickWand*
    T_NUMBER(MagickRollImage, 1)     // ssize_t
    T_NUMBER(MagickRollImage, 2)     // ssize_t

    RETURN_NUMBER(MagickRollImage((MagickWand *)(long)PARAM(0), (ssize_t)PARAM(1), (ssize_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickRotateImage, 3)
    T_HANDLE(MagickRotateImage, 0)     // MagickWand*
    T_HANDLE(MagickRotateImage, 1)     // PixelWand*
    T_NUMBER(MagickRotateImage, 2)     // double

    RETURN_NUMBER(MagickRotateImage((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSampleImage, 3)
    T_HANDLE(MagickSampleImage, 0)     // MagickWand*
    T_NUMBER(MagickSampleImage, 1)     // size_t
    T_NUMBER(MagickSampleImage, 2)     // size_t

    RETURN_NUMBER(MagickSampleImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickScaleImage, 3)
    T_HANDLE(MagickScaleImage, 0)     // MagickWand*
    T_NUMBER(MagickScaleImage, 1)     // size_t
    T_NUMBER(MagickScaleImage, 2)     // size_t

    RETURN_NUMBER(MagickScaleImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSegmentImage, 5)
    T_HANDLE(MagickSegmentImage, 0)     // MagickWand*
    T_NUMBER(MagickSegmentImage, 1)     // ColorspaceType
    T_NUMBER(MagickSegmentImage, 2)     // MagickBooleanType
    T_NUMBER(MagickSegmentImage, 3)     // double
    T_NUMBER(MagickSegmentImage, 4)     // double

    RETURN_NUMBER(MagickSegmentImage((MagickWand *)(long)PARAM(0), (ColorspaceType)PARAM(1), (MagickBooleanType)PARAM(2), (double)PARAM(3), (double)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSelectiveBlurImage, 4)
    T_HANDLE(MagickSelectiveBlurImage, 0)     // MagickWand*
    T_NUMBER(MagickSelectiveBlurImage, 1)     // double
    T_NUMBER(MagickSelectiveBlurImage, 2)     // double
    T_NUMBER(MagickSelectiveBlurImage, 3)     // double

    RETURN_NUMBER(MagickSelectiveBlurImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSelectiveBlurImageChannel, 5)
    T_HANDLE(MagickSelectiveBlurImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickSelectiveBlurImageChannel, 1)     // ChannelType
    T_NUMBER(MagickSelectiveBlurImageChannel, 2)     // double
    T_NUMBER(MagickSelectiveBlurImageChannel, 3)     // double
    T_NUMBER(MagickSelectiveBlurImageChannel, 4)     // double

    RETURN_NUMBER(MagickSelectiveBlurImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSeparateImageChannel, 2)
    T_HANDLE(MagickSeparateImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickSeparateImageChannel, 1)     // ChannelType

    RETURN_NUMBER(MagickSeparateImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSepiaToneImage, 2)
    T_HANDLE(MagickSepiaToneImage, 0)     // MagickWand*
    T_NUMBER(MagickSepiaToneImage, 1)     // double

    RETURN_NUMBER(MagickSepiaToneImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImage, 2)
    T_HANDLE(MagickSetImage, 0)     // MagickWand*
    T_HANDLE(MagickSetImage, 1)     // MagickWand*

    RETURN_NUMBER(MagickSetImage((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageAlphaChannel, 2)
    T_HANDLE(MagickSetImageAlphaChannel, 0)     // MagickWand*
    T_NUMBER(MagickSetImageAlphaChannel, 1)     // AlphaChannelType

    RETURN_NUMBER(MagickSetImageAlphaChannel((MagickWand *)(long)PARAM(0), (AlphaChannelType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageBackgroundColor, 2)
    T_HANDLE(MagickSetImageBackgroundColor, 0)     // MagickWand*
    T_HANDLE(MagickSetImageBackgroundColor, 1)     // PixelWand*

    RETURN_NUMBER(MagickSetImageBackgroundColor((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageBias, 2)
    T_HANDLE(MagickSetImageBias, 0)     // MagickWand*
    T_NUMBER(MagickSetImageBias, 1)     // double

    RETURN_NUMBER(MagickSetImageBias((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageBluePrimary, 3)
    T_HANDLE(MagickSetImageBluePrimary, 0)     // MagickWand*
    T_NUMBER(MagickSetImageBluePrimary, 1)     // double
    T_NUMBER(MagickSetImageBluePrimary, 2)     // double

    RETURN_NUMBER(MagickSetImageBluePrimary((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageBorderColor, 2)
    T_HANDLE(MagickSetImageBorderColor, 0)     // MagickWand*
    T_HANDLE(MagickSetImageBorderColor, 1)     // PixelWand*

    RETURN_NUMBER(MagickSetImageBorderColor((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageChannelDepth, 3)
    T_HANDLE(MagickSetImageChannelDepth, 0)     // MagickWand*
    T_NUMBER(MagickSetImageChannelDepth, 1)     // ChannelType
    T_NUMBER(MagickSetImageChannelDepth, 2)     // size_t

    RETURN_NUMBER(MagickSetImageChannelDepth((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageClipMask, 2)
    T_HANDLE(MagickSetImageClipMask, 0)     // MagickWand*
    T_HANDLE(MagickSetImageClipMask, 1)     // MagickWand*

    RETURN_NUMBER(MagickSetImageClipMask((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageColor, 2)
    T_HANDLE(MagickSetImageColor, 0)     // MagickWand*
    T_HANDLE(MagickSetImageColor, 1)     // PixelWand*

    RETURN_NUMBER(MagickSetImageColor((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageColormapColor, 3)
    T_HANDLE(MagickSetImageColormapColor, 0)     // MagickWand*
    T_NUMBER(MagickSetImageColormapColor, 1)     // size_t
    T_HANDLE(MagickSetImageColormapColor, 2)     // PixelWand*

    RETURN_NUMBER(MagickSetImageColormapColor((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (PixelWand *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageColorspace, 2)
    T_HANDLE(MagickSetImageColorspace, 0)     // MagickWand*
    T_NUMBER(MagickSetImageColorspace, 1)     // ColorspaceType

    RETURN_NUMBER(MagickSetImageColorspace((MagickWand *)(long)PARAM(0), (ColorspaceType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageCompose, 2)
    T_HANDLE(MagickSetImageCompose, 0)     // MagickWand*
    T_NUMBER(MagickSetImageCompose, 1)     // CompositeOperator

    RETURN_NUMBER(MagickSetImageCompose((MagickWand *)(long)PARAM(0), (CompositeOperator)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageCompression, 2)
    T_HANDLE(MagickSetImageCompression, 0)     // MagickWand*
    T_NUMBER(MagickSetImageCompression, 1)     // CompressionType

    RETURN_NUMBER(MagickSetImageCompression((MagickWand *)(long)PARAM(0), (CompressionType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageDelay, 2)
    T_HANDLE(MagickSetImageDelay, 0)     // MagickWand*
    T_NUMBER(MagickSetImageDelay, 1)     // size_t

    RETURN_NUMBER(MagickSetImageDelay((MagickWand *)(long)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageDepth, 2)
    T_HANDLE(MagickSetImageDepth, 0)     // MagickWand*
    T_NUMBER(MagickSetImageDepth, 1)     // size_t

    RETURN_NUMBER(MagickSetImageDepth((MagickWand *)(long)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageDispose, 2)
    T_HANDLE(MagickSetImageDispose, 0)     // MagickWand*
    T_NUMBER(MagickSetImageDispose, 1)     // DisposeType

    RETURN_NUMBER(MagickSetImageDispose((MagickWand *)(long)PARAM(0), (DisposeType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageCompressionQuality, 2)
    T_HANDLE(MagickSetImageCompressionQuality, 0)     // MagickWand*
    T_NUMBER(MagickSetImageCompressionQuality, 1)     // size_t

    RETURN_NUMBER(MagickSetImageCompressionQuality((MagickWand *)(long)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageExtent, 3)
    T_HANDLE(MagickSetImageExtent, 0)     // MagickWand*
    T_NUMBER(MagickSetImageExtent, 1)     // size_t
    T_NUMBER(MagickSetImageExtent, 2)     // size_t

    RETURN_NUMBER(MagickSetImageExtent((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageFilename, 2)
    T_HANDLE(MagickSetImageFilename, 0)     // MagickWand*
    T_STRING(MagickSetImageFilename, 1)     // char*

    RETURN_NUMBER(MagickSetImageFilename((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageFormat, 2)
    T_HANDLE(MagickSetImageFormat, 0)     // MagickWand*
    T_STRING(MagickSetImageFormat, 1)     // char*

    RETURN_NUMBER(MagickSetImageFormat((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageFuzz, 2)
    T_HANDLE(MagickSetImageFuzz, 0)     // MagickWand*
    T_NUMBER(MagickSetImageFuzz, 1)     // double

    RETURN_NUMBER(MagickSetImageFuzz((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageGamma, 2)
    T_HANDLE(MagickSetImageGamma, 0)     // MagickWand*
    T_NUMBER(MagickSetImageGamma, 1)     // double

    RETURN_NUMBER(MagickSetImageGamma((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageGravity, 2)
    T_HANDLE(MagickSetImageGravity, 0)     // MagickWand*
    T_NUMBER(MagickSetImageGravity, 1)     // GravityType

    RETURN_NUMBER(MagickSetImageGravity((MagickWand *)(long)PARAM(0), (GravityType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageGreenPrimary, 3)
    T_HANDLE(MagickSetImageGreenPrimary, 0)     // MagickWand*
    T_NUMBER(MagickSetImageGreenPrimary, 1)     // double
    T_NUMBER(MagickSetImageGreenPrimary, 2)     // double

    RETURN_NUMBER(MagickSetImageGreenPrimary((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageInterlaceScheme, 2)
    T_HANDLE(MagickSetImageInterlaceScheme, 0)     // MagickWand*
    T_NUMBER(MagickSetImageInterlaceScheme, 1)     // InterlaceType

    RETURN_NUMBER(MagickSetImageInterlaceScheme((MagickWand *)(long)PARAM(0), (InterlaceType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageInterpolateMethod, 2)
    T_HANDLE(MagickSetImageInterpolateMethod, 0)     // MagickWand*
    T_NUMBER(MagickSetImageInterpolateMethod, 1)     // InterpolatePixelMethod

    RETURN_NUMBER(MagickSetImageInterpolateMethod((MagickWand *)(long)PARAM(0), (InterpolatePixelMethod)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageIterations, 2)
    T_HANDLE(MagickSetImageIterations, 0)     // MagickWand*
    T_NUMBER(MagickSetImageIterations, 1)     // size_t

    RETURN_NUMBER(MagickSetImageIterations((MagickWand *)(long)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageMatte, 2)
    T_HANDLE(MagickSetImageMatte, 0)     // MagickWand*
    T_NUMBER(MagickSetImageMatte, 1)     // MagickBooleanType

    RETURN_NUMBER(MagickSetImageMatte((MagickWand *)(long)PARAM(0), (MagickBooleanType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageMatteColor, 2)
    T_HANDLE(MagickSetImageMatteColor, 0)     // MagickWand*
    T_HANDLE(MagickSetImageMatteColor, 1)     // PixelWand*

    RETURN_NUMBER(MagickSetImageMatteColor((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageOpacity, 2)
    T_HANDLE(MagickSetImageOpacity, 0)     // MagickWand*
    T_NUMBER(MagickSetImageOpacity, 1)     // double

    RETURN_NUMBER(MagickSetImageOpacity((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageOrientation, 2)
    T_HANDLE(MagickSetImageOrientation, 0)     // MagickWand*
    T_NUMBER(MagickSetImageOrientation, 1)     // OrientationType

    RETURN_NUMBER(MagickSetImageOrientation((MagickWand *)(long)PARAM(0), (OrientationType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImagePage, 5)
    T_HANDLE(MagickSetImagePage, 0)     // MagickWand*
    T_NUMBER(MagickSetImagePage, 1)     // size_t
    T_NUMBER(MagickSetImagePage, 2)     // size_t
    T_NUMBER(MagickSetImagePage, 3)     // ssize_t
    T_NUMBER(MagickSetImagePage, 4)     // ssize_t

    RETURN_NUMBER(MagickSetImagePage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageRedPrimary, 3)
    T_HANDLE(MagickSetImageRedPrimary, 0)     // MagickWand*
    T_NUMBER(MagickSetImageRedPrimary, 1)     // double
    T_NUMBER(MagickSetImageRedPrimary, 2)     // double

    RETURN_NUMBER(MagickSetImageRedPrimary((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageRenderingIntent, 2)
    T_HANDLE(MagickSetImageRenderingIntent, 0)     // MagickWand*
    T_NUMBER(MagickSetImageRenderingIntent, 1)     // RenderingIntent

    RETURN_NUMBER(MagickSetImageRenderingIntent((MagickWand *)(long)PARAM(0), (RenderingIntent)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageResolution, 3)
    T_HANDLE(MagickSetImageResolution, 0)     // MagickWand*
    T_NUMBER(MagickSetImageResolution, 1)     // double
    T_NUMBER(MagickSetImageResolution, 2)     // double

    RETURN_NUMBER(MagickSetImageResolution((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageScene, 2)
    T_HANDLE(MagickSetImageScene, 0)     // MagickWand*
    T_NUMBER(MagickSetImageScene, 1)     // size_t

    RETURN_NUMBER(MagickSetImageScene((MagickWand *)(long)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageTicksPerSecond, 2)
    T_HANDLE(MagickSetImageTicksPerSecond, 0)     // MagickWand*
    T_NUMBER(MagickSetImageTicksPerSecond, 1)     // ssize_t

    RETURN_NUMBER(MagickSetImageTicksPerSecond((MagickWand *)(long)PARAM(0), (ssize_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageType, 2)
    T_HANDLE(MagickSetImageType, 0)     // MagickWand*
    T_NUMBER(MagickSetImageType, 1)     // ImageType

    RETURN_NUMBER(MagickSetImageType((MagickWand *)(long)PARAM(0), (ImageType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageUnits, 2)
    T_HANDLE(MagickSetImageUnits, 0)     // MagickWand*
    T_NUMBER(MagickSetImageUnits, 1)     // ResolutionType

    RETURN_NUMBER(MagickSetImageUnits((MagickWand *)(long)PARAM(0), (ResolutionType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageWhitePoint, 3)
    T_HANDLE(MagickSetImageWhitePoint, 0)     // MagickWand*
    T_NUMBER(MagickSetImageWhitePoint, 1)     // double
    T_NUMBER(MagickSetImageWhitePoint, 2)     // double

    RETURN_NUMBER(MagickSetImageWhitePoint((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickShadeImage, 4)
    T_HANDLE(MagickShadeImage, 0)     // MagickWand*
    T_NUMBER(MagickShadeImage, 1)     // MagickBooleanType
    T_NUMBER(MagickShadeImage, 2)     // double
    T_NUMBER(MagickShadeImage, 3)     // double

    RETURN_NUMBER(MagickShadeImage((MagickWand *)(long)PARAM(0), (MagickBooleanType)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickShadowImage, 5)
    T_HANDLE(MagickShadowImage, 0)     // MagickWand*
    T_NUMBER(MagickShadowImage, 1)     // double
    T_NUMBER(MagickShadowImage, 2)     // double
    T_NUMBER(MagickShadowImage, 3)     // ssize_t
    T_NUMBER(MagickShadowImage, 4)     // ssize_t

    RETURN_NUMBER(MagickShadowImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSharpenImage, 3)
    T_HANDLE(MagickSharpenImage, 0)     // MagickWand*
    T_NUMBER(MagickSharpenImage, 1)     // double
    T_NUMBER(MagickSharpenImage, 2)     // double

    RETURN_NUMBER(MagickSharpenImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSharpenImageChannel, 4)
    T_HANDLE(MagickSharpenImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickSharpenImageChannel, 1)     // ChannelType
    T_NUMBER(MagickSharpenImageChannel, 2)     // double
    T_NUMBER(MagickSharpenImageChannel, 3)     // double

    RETURN_NUMBER(MagickSharpenImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickShaveImage, 3)
    T_HANDLE(MagickShaveImage, 0)     // MagickWand*
    T_NUMBER(MagickShaveImage, 1)     // size_t
    T_NUMBER(MagickShaveImage, 2)     // size_t

    RETURN_NUMBER(MagickShaveImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickShearImage, 4)
    T_HANDLE(MagickShearImage, 0)     // MagickWand*
    T_HANDLE(MagickShearImage, 1)     // PixelWand*
    T_NUMBER(MagickShearImage, 2)     // double
    T_NUMBER(MagickShearImage, 3)     // double

    RETURN_NUMBER(MagickShearImage((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSigmoidalContrastImage, 4)
    T_HANDLE(MagickSigmoidalContrastImage, 0)     // MagickWand*
    T_NUMBER(MagickSigmoidalContrastImage, 1)     // MagickBooleanType
    T_NUMBER(MagickSigmoidalContrastImage, 2)     // double
    T_NUMBER(MagickSigmoidalContrastImage, 3)     // double

    RETURN_NUMBER(MagickSigmoidalContrastImage((MagickWand *)(long)PARAM(0), (MagickBooleanType)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSigmoidalContrastImageChannel, 5)
    T_HANDLE(MagickSigmoidalContrastImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickSigmoidalContrastImageChannel, 1)     // ChannelType
    T_NUMBER(MagickSigmoidalContrastImageChannel, 2)     // MagickBooleanType
    T_NUMBER(MagickSigmoidalContrastImageChannel, 3)     // double
    T_NUMBER(MagickSigmoidalContrastImageChannel, 4)     // double

    RETURN_NUMBER(MagickSigmoidalContrastImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (MagickBooleanType)PARAM(2), (double)PARAM(3), (double)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSketchImage, 4)
    T_HANDLE(MagickSketchImage, 0)     // MagickWand*
    T_NUMBER(MagickSketchImage, 1)     // double
    T_NUMBER(MagickSketchImage, 2)     // double
    T_NUMBER(MagickSketchImage, 3)     // double

    RETURN_NUMBER(MagickSketchImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSolarizeImage, 2)
    T_HANDLE(MagickSolarizeImage, 0)     // MagickWand*
    T_NUMBER(MagickSolarizeImage, 1)     // double

    RETURN_NUMBER(MagickSolarizeImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSparseColorImage, 5)
    T_HANDLE(MagickSparseColorImage, 0)     // MagickWand*
    T_NUMBER(MagickSparseColorImage, 1)     // ChannelType
    T_NUMBER(MagickSparseColorImage, 2)     // SparseColorMethod
    T_NUMBER(MagickSparseColorImage, 3)     // size_t

// ... parameter 4 is by reference (double*)
    double local_parameter_4;

    RETURN_NUMBER(MagickSparseColorImage((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (SparseColorMethod)PARAM(2), (size_t)PARAM(3), &local_parameter_4))
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSpliceImage, 5)
    T_HANDLE(MagickSpliceImage, 0)     // MagickWand*
    T_NUMBER(MagickSpliceImage, 1)     // size_t
    T_NUMBER(MagickSpliceImage, 2)     // size_t
    T_NUMBER(MagickSpliceImage, 3)     // ssize_t
    T_NUMBER(MagickSpliceImage, 4)     // ssize_t

    RETURN_NUMBER(MagickSpliceImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSpreadImage, 2)
    T_HANDLE(MagickSpreadImage, 0)     // MagickWand*
    T_NUMBER(MagickSpreadImage, 1)     // double

    RETURN_NUMBER(MagickSpreadImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickStripImage, 1)
    T_HANDLE(MagickStripImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickStripImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSwirlImage, 2)
    T_HANDLE(MagickSwirlImage, 0)     // MagickWand*
    T_NUMBER(MagickSwirlImage, 1)     // double

    RETURN_NUMBER(MagickSwirlImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickTintImage, 3)
    T_HANDLE(MagickTintImage, 0)     // MagickWand*
    T_HANDLE(MagickTintImage, 1)     // PixelWand*
    T_HANDLE(MagickTintImage, 2)     // PixelWand*

    RETURN_NUMBER(MagickTintImage((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1), (PixelWand *)(long)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickTransformImageColorspace, 2)
    T_HANDLE(MagickTransformImageColorspace, 0)     // MagickWand*
    T_NUMBER(MagickTransformImageColorspace, 1)     // ColorspaceType

    RETURN_NUMBER(MagickTransformImageColorspace((MagickWand *)(long)PARAM(0), (ColorspaceType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickTransposeImage, 1)
    T_HANDLE(MagickTransposeImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickTransposeImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickTransverseImage, 1)
    T_HANDLE(MagickTransverseImage, 0)     // MagickWand*

    RETURN_NUMBER(MagickTransverseImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickThresholdImage, 2)
    T_HANDLE(MagickThresholdImage, 0)     // MagickWand*
    T_NUMBER(MagickThresholdImage, 1)     // double

    RETURN_NUMBER(MagickThresholdImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickThresholdImageChannel, 3)
    T_HANDLE(MagickThresholdImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickThresholdImageChannel, 1)     // ChannelType
    T_NUMBER(MagickThresholdImageChannel, 2)     // double

    RETURN_NUMBER(MagickThresholdImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickThumbnailImage, 3)
    T_HANDLE(MagickThumbnailImage, 0)     // MagickWand*
    T_NUMBER(MagickThumbnailImage, 1)     // size_t
    T_NUMBER(MagickThumbnailImage, 2)     // size_t

    RETURN_NUMBER(MagickThumbnailImage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickTrimImage, 2)
    T_HANDLE(MagickTrimImage, 0)     // MagickWand*
    T_NUMBER(MagickTrimImage, 1)     // double

    RETURN_NUMBER(MagickTrimImage((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickUniqueImageColors, 1)
    T_HANDLE(MagickUniqueImageColors, 0)     // MagickWand*

    RETURN_NUMBER(MagickUniqueImageColors((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickUnsharpMaskImage, 5)
    T_HANDLE(MagickUnsharpMaskImage, 0)     // MagickWand*
    T_NUMBER(MagickUnsharpMaskImage, 1)     // double
    T_NUMBER(MagickUnsharpMaskImage, 2)     // double
    T_NUMBER(MagickUnsharpMaskImage, 3)     // double
    T_NUMBER(MagickUnsharpMaskImage, 4)     // double

    RETURN_NUMBER(MagickUnsharpMaskImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickUnsharpMaskImageChannel, 6)
    T_HANDLE(MagickUnsharpMaskImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickUnsharpMaskImageChannel, 1)     // ChannelType
    T_NUMBER(MagickUnsharpMaskImageChannel, 2)     // double
    T_NUMBER(MagickUnsharpMaskImageChannel, 3)     // double
    T_NUMBER(MagickUnsharpMaskImageChannel, 4)     // double
    T_NUMBER(MagickUnsharpMaskImageChannel, 5)     // double

    RETURN_NUMBER(MagickUnsharpMaskImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4), (double)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickVignetteImage, 5)
    T_HANDLE(MagickVignetteImage, 0)     // MagickWand*
    T_NUMBER(MagickVignetteImage, 1)     // double
    T_NUMBER(MagickVignetteImage, 2)     // double
    T_NUMBER(MagickVignetteImage, 3)     // ssize_t
    T_NUMBER(MagickVignetteImage, 4)     // ssize_t

    RETURN_NUMBER(MagickVignetteImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickWaveImage, 3)
    T_HANDLE(MagickWaveImage, 0)     // MagickWand*
    T_NUMBER(MagickWaveImage, 1)     // double
    T_NUMBER(MagickWaveImage, 2)     // double

    RETURN_NUMBER(MagickWaveImage((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickWhiteThresholdImage, 2)
    T_HANDLE(MagickWhiteThresholdImage, 0)     // MagickWand*
    T_HANDLE(MagickWhiteThresholdImage, 1)     // PixelWand*

    RETURN_NUMBER(MagickWhiteThresholdImage((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickWriteImage, 2)
    T_HANDLE(MagickWriteImage, 0)     // MagickWand*
    T_STRING(MagickWriteImage, 1)     // char*

    RETURN_NUMBER(MagickWriteImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickWriteImageFile, 2)
    T_HANDLE(MagickWriteImageFile, 0)     // MagickWand*
    T_HANDLE(MagickWriteImageFile, 1)     // FILE*

    RETURN_NUMBER(MagickWriteImageFile((MagickWand *)(long)PARAM(0), (FILE *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickWriteImagesFile, 2)
    T_HANDLE(MagickWriteImagesFile, 0)     // MagickWand*
    T_HANDLE(MagickWriteImagesFile, 1)     // FILE*

    RETURN_NUMBER(MagickWriteImagesFile((MagickWand *)(long)PARAM(0), (FILE *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageChannelFeatures, 2)
    T_HANDLE(MagickGetImageChannelFeatures, 0)     // MagickWand*
    T_NUMBER(MagickGetImageChannelFeatures, 1)     // size_t

    ChannelFeatures * cf = MagickGetImageChannelFeatures((MagickWand *)(long)PARAM(0), (size_t)PARAM(1));
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"angular_second_moment[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->angular_second_moment[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"angular_second_moment[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->angular_second_moment[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"angular_second_moment[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->angular_second_moment[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"angular_second_moment[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->angular_second_moment[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"contrast[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->contrast[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"contrast[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->contrast[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"contrast[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->contrast[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"contrast[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->contrast[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"correlation[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->correlation[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"correlation[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->correlation[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"correlation[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->correlation[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"correlation[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->correlation[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"variance_sum_of_squares[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->variance_sum_of_squares[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"variance_sum_of_squares[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->variance_sum_of_squares[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"variance_sum_of_squares[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->variance_sum_of_squares[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"variance_sum_of_squares[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->variance_sum_of_squares[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"inverse_difference_moment[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->inverse_difference_moment[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"inverse_difference_moment[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->inverse_difference_moment[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"inverse_difference_moment[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->inverse_difference_moment[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"inverse_difference_moment[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->inverse_difference_moment[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_average[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_average[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_average[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_average[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_average[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_average[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_average[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_average[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_variance[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_variance[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_variance[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_variance[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_variance[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_variance[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_variance[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_variance[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_entropy[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_entropy[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_entropy[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_entropy[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_entropy[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_entropy[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_entropy[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->sum_entropy[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"entropy[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->entropy[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"entropy[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->entropy[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"entropy[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->entropy[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"entropy[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->entropy[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"difference_variance[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->difference_variance[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"difference_variance[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->difference_variance[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"difference_variance[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->difference_variance[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"difference_variance[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->difference_variance[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"difference_entropy[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->difference_entropy[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"difference_entropy[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->difference_entropy[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"difference_entropy[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->difference_entropy[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"difference_entropy[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->difference_entropy[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"measure_of_correlation_1[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->measure_of_correlation_1[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"measure_of_correlation_1[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->measure_of_correlation_1[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"measure_of_correlation_1[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->measure_of_correlation_1[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"measure_of_correlation_1[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->measure_of_correlation_1[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"measure_of_correlation_2[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->measure_of_correlation_2[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"measure_of_correlation_2[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->measure_of_correlation_2[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"measure_of_correlation_2[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->measure_of_correlation_2[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"measure_of_correlation_2[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->measure_of_correlation_2[3]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"maximum_correlation_coefficient[0]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->maximum_correlation_coefficient[0]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"maximum_correlation_coefficient[1]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->maximum_correlation_coefficient[1]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"maximum_correlation_coefficient[2]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->maximum_correlation_coefficient[2]);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"maximum_correlation_coefficient[3]", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cf->maximum_correlation_coefficient[3]);
    MagickRelinquishMemory(cf);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageChannelStatistics, 1)
    T_HANDLE(MagickGetImageChannelStatistics, 0)     // MagickWand*

    ChannelStatistics * cs = MagickGetImageChannelStatistics((MagickWand *)(long)PARAM(0));

    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"depth", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cs->depth);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"minima", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cs->minima);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"maxima", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cs->maxima);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cs->sum);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_squared", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cs->sum_squared);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"sum_cubed", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cs->sum_cubed);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"mean", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cs->mean);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"variance", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cs->variance);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"standard_deviation", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cs->standard_deviation);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"kurtosis", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cs->kurtosis);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, (char *)"skewness", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)cs->skewness);

    MagickRelinquishMemory(cs);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageFilename, 1)
    T_HANDLE(MagickGetImageFilename, 0)     // MagickWand*

    RETURN_STRING2((char *)MagickGetImageFilename((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageFormat, 1)
    T_HANDLE(MagickGetImageFormat, 0)     // MagickWand*

    RETURN_STRING2((char *)MagickGetImageFormat((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageSignature, 1)
    T_HANDLE(MagickGetImageSignature, 0)     // MagickWand*

    RETURN_STRING2((char *)MagickGetImageSignature((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickIdentifyImage, 1)
    T_HANDLE(MagickIdentifyImage, 0)     // MagickWand*

    RETURN_STRING2((char *)MagickIdentifyImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageColorspace, 1)
    T_HANDLE(MagickGetImageColorspace, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageColorspace((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageCompose, 1)
    T_HANDLE(MagickGetImageCompose, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageCompose((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageCompression, 1)
    T_HANDLE(MagickGetImageCompression, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageCompression((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageDispose, 1)
    T_HANDLE(MagickGetImageDispose, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageDispose((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageChannelDistortions, 3)
    T_HANDLE(MagickGetImageChannelDistortions, 0)     // MagickWand*
    T_HANDLE(MagickGetImageChannelDistortions, 1)     // MagickWand*
    T_NUMBER(MagickGetImageChannelDistortions, 2)     // MetricType

    double *ret = MagickGetImageChannelDistortions((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1), (MetricType)PARAM(2));
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < AllChannels; i++)
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)ret[i]);
    IMAGICK_FREE_MEMORY(ret);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageFuzz, 1)
    T_HANDLE(MagickGetImageFuzz, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageFuzz((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageGamma, 1)
    T_HANDLE(MagickGetImageGamma, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageGamma((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageTotalInkDensity, 1)
    T_HANDLE(MagickGetImageTotalInkDensity, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageTotalInkDensity((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageGravity, 1)
    T_HANDLE(MagickGetImageGravity, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageGravity((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDestroyImage, 1)
    T_HANDLE(MagickDestroyImage, 0)

    RETURN_NUMBER((long)MagickDestroyImage((Image *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(GetImageFromMagickWand, 1)
    T_HANDLE(GetImageFromMagickWand, 0)     // MagickWand*

    RETURN_NUMBER((long)GetImageFromMagickWand((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageType, 1)
    T_HANDLE(MagickGetImageType, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageType((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageInterlaceScheme, 1)
    T_HANDLE(MagickGetImageInterlaceScheme, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageInterlaceScheme((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageInterpolateMethod, 1)
    T_HANDLE(MagickGetImageInterpolateMethod, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageInterpolateMethod((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickAppendImages, 2)
    T_HANDLE(MagickAppendImages, 0)     // MagickWand*
    T_NUMBER(MagickAppendImages, 1)     // MagickBooleanType

    RETURN_NUMBER((long)MagickAppendImages((MagickWand *)(long)PARAM(0), (MagickBooleanType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickCoalesceImages, 1)
    T_HANDLE(MagickCoalesceImages, 0)     // MagickWand*

    RETURN_NUMBER((long)MagickCoalesceImages((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickCombineImages, 2)
    T_HANDLE(MagickCombineImages, 0)     // MagickWand*
    T_NUMBER(MagickCombineImages, 1)     // ChannelType

    RETURN_NUMBER((long)MagickCombineImages((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickCompareImageChannels, 5)
    T_HANDLE(MagickCompareImageChannels, 0)     // MagickWand*
    T_HANDLE(MagickCompareImageChannels, 1)     // MagickWand*
    T_NUMBER(MagickCompareImageChannels, 2)     // ChannelType
    T_NUMBER(MagickCompareImageChannels, 3)     // MetricType

// ... parameter 4 is by reference (double*)
    double local_parameter_4;

    RETURN_NUMBER((long)MagickCompareImageChannels((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1), (ChannelType)PARAM(2), (MetricType)PARAM(3), &local_parameter_4))
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickCompareImages, 4)
    T_HANDLE(MagickCompareImages, 0)     // MagickWand*
    T_HANDLE(MagickCompareImages, 1)     // MagickWand*
    T_NUMBER(MagickCompareImages, 2)     // MetricType

// ... parameter 3 is by reference (double*)
    double local_parameter_3;

    RETURN_NUMBER((long)MagickCompareImages((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1), (MetricType)PARAM(2), &local_parameter_3))
    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickCompareImageLayers, 2)
    T_HANDLE(MagickCompareImageLayers, 0)     // MagickWand*
    T_NUMBER(MagickCompareImageLayers, 1)     // ImageLayerMethod

    RETURN_NUMBER((long)MagickCompareImageLayers((MagickWand *)(long)PARAM(0), (ImageLayerMethod)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDeconstructImages, 1)
    T_HANDLE(MagickDeconstructImages, 0)     // MagickWand*

    RETURN_NUMBER((long)MagickDeconstructImages((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickEvaluateImages, 2)
    T_HANDLE(MagickEvaluateImages, 0)     // MagickWand*
    T_NUMBER(MagickEvaluateImages, 1)     // MagickEvaluateOperator

    RETURN_NUMBER((long)MagickEvaluateImages((MagickWand *)(long)PARAM(0), (MagickEvaluateOperator)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickFxImage, 2)
    T_HANDLE(MagickFxImage, 0)     // MagickWand*
    T_STRING(MagickFxImage, 1)     // char*

    RETURN_NUMBER((long)MagickFxImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickFxImageChannel, 3)
    T_HANDLE(MagickFxImageChannel, 0)     // MagickWand*
    T_NUMBER(MagickFxImageChannel, 1)     // ChannelType
    T_STRING(MagickFxImageChannel, 2)     // char*

    RETURN_NUMBER((long)MagickFxImageChannel((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImage, 1)
    T_HANDLE(MagickGetImage, 0)     // MagickWand*

    RETURN_NUMBER((long)MagickGetImage((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageClipMask, 1)
    T_HANDLE(MagickGetImageClipMask, 0)     // MagickWand*

    RETURN_NUMBER((long)MagickGetImageClipMask((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageRegion, 5)
    T_HANDLE(MagickGetImageRegion, 0)     // MagickWand*
    T_NUMBER(MagickGetImageRegion, 1)     // size_t
    T_NUMBER(MagickGetImageRegion, 2)     // size_t
    T_NUMBER(MagickGetImageRegion, 3)     // ssize_t
    T_NUMBER(MagickGetImageRegion, 4)     // ssize_t

    RETURN_NUMBER((long)MagickGetImageRegion((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickMergeImageLayers, 2)
    T_HANDLE(MagickMergeImageLayers, 0)     // MagickWand*
    T_NUMBER(MagickMergeImageLayers, 1)     // ImageLayerMethod

    RETURN_NUMBER((long)MagickMergeImageLayers((MagickWand *)(long)PARAM(0), (ImageLayerMethod)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickMorphImages, 2)
    T_HANDLE(MagickMorphImages, 0)     // MagickWand*
    T_NUMBER(MagickMorphImages, 1)     // size_t

    RETURN_NUMBER((long)MagickMorphImages((MagickWand *)(long)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickMontageImage, 6)
    T_HANDLE(MagickMontageImage, 0)     // MagickWand*
    T_HANDLE(MagickMontageImage, 1)     // DrawingWand*
    T_STRING(MagickMontageImage, 2)     // char*
    T_STRING(MagickMontageImage, 3)     // char*
    T_NUMBER(MagickMontageImage, 4)     // MontageMode
    T_STRING(MagickMontageImage, 5)     // char*

    RETURN_NUMBER((long)MagickMontageImage((MagickWand *)(long)PARAM(0), (DrawingWand *)(long)PARAM(1), (char *)PARAM(2), (char *)PARAM(3), (MontageMode)PARAM(4), (char *)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickOptimizeImageLayers, 1)
    T_HANDLE(MagickOptimizeImageLayers, 0)     // MagickWand*

    RETURN_NUMBER((long)MagickOptimizeImageLayers((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickPreviewImages, 2)
    T_HANDLE(MagickPreviewImages, 0)     // MagickWand*
    T_NUMBER(MagickPreviewImages, 1)     // PreviewType

    RETURN_NUMBER((long)MagickPreviewImages((MagickWand *)(long)PARAM(0), (PreviewType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSimilarityImage, 4)
    T_HANDLE(MagickSimilarityImage, 0)     // MagickWand*
    T_HANDLE(MagickSimilarityImage, 1)     // MagickWand*

// ... parameter 2 is by reference (RectangleInfo*)
    RectangleInfo local_parameter_2;
// ... parameter 3 is by reference (double*)
    double local_parameter_3;

    RETURN_NUMBER((long)MagickSimilarityImage((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1), &local_parameter_2, &local_parameter_3))

    Invoke(INVOKE_CREATE_ARRAY, PARAMETER(2), 0);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(2), (char *)"x", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)local_parameter_2.x);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(2), (char *)"y", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)local_parameter_2.y);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(2), (char *)"width", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)local_parameter_2.width);
    Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, PARAMETER(2), (char *)"height", (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)local_parameter_2.height);

    SET_NUMBER(3, (long)local_parameter_3)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSteganoImage, 3)
    T_HANDLE(MagickSteganoImage, 0)     // MagickWand*
    T_HANDLE(MagickSteganoImage, 1)     // MagickWand*
    T_NUMBER(MagickSteganoImage, 2)     // ssize_t

    RETURN_NUMBER((long)MagickSteganoImage((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1), (ssize_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickStereoImage, 2)
    T_HANDLE(MagickStereoImage, 0)     // MagickWand*
    T_HANDLE(MagickStereoImage, 1)     // MagickWand*

    RETURN_NUMBER((long)MagickStereoImage((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickTextureImage, 2)
    T_HANDLE(MagickTextureImage, 0)     // MagickWand*
    T_HANDLE(MagickTextureImage, 1)     // MagickWand*

    RETURN_NUMBER((long)MagickTextureImage((MagickWand *)(long)PARAM(0), (MagickWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickTransformImage, 3)
    T_HANDLE(MagickTransformImage, 0)     // MagickWand*
    T_STRING(MagickTransformImage, 1)     // char*
    T_STRING(MagickTransformImage, 2)     // char*

    RETURN_NUMBER((long)MagickTransformImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageOrientation, 1)
    T_HANDLE(MagickGetImageOrientation, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageOrientation((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageHistogram, 1)
    T_HANDLE(MagickGetImageHistogram, 0)     // MagickWand*

    size_t local_parameter_1;

    PixelWand **ret = MagickGetImageHistogram((MagickWand *)(long)PARAM(0), &local_parameter_1);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_1; i++)
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(long)ret[i]);
    IMAGICK_FREE_MEMORY(ret)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageRenderingIntent, 1)
    T_HANDLE(MagickGetImageRenderingIntent, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageRenderingIntent((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageUnits, 1)
    T_HANDLE(MagickGetImageUnits, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageUnits((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageColors, 1)
    T_HANDLE(MagickGetImageColors, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageColors((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageCompressionQuality, 1)
    T_HANDLE(MagickGetImageCompressionQuality, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageCompressionQuality((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageDelay, 1)
    T_HANDLE(MagickGetImageDelay, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageDelay((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageChannelDepth, 2)
    T_HANDLE(MagickGetImageChannelDepth, 0)     // MagickWand*
    T_NUMBER(MagickGetImageChannelDepth, 1)     // ChannelType

    RETURN_NUMBER(MagickGetImageChannelDepth((MagickWand *)(long)PARAM(0), (ChannelType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageDepth, 1)
    T_HANDLE(MagickGetImageDepth, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageDepth((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageHeight, 1)
    T_HANDLE(MagickGetImageHeight, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageHeight((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageIterations, 1)
    T_HANDLE(MagickGetImageIterations, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageIterations((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageScene, 1)
    T_HANDLE(MagickGetImageScene, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageScene((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageTicksPerSecond, 1)
    T_HANDLE(MagickGetImageTicksPerSecond, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageTicksPerSecond((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageWidth, 1)
    T_HANDLE(MagickGetImageWidth, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageWidth((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetNumberImages, 1)
    T_HANDLE(MagickGetNumberImages, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetNumberImages((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageBlob, 1)
    T_HANDLE(MagickGetImageBlob, 0)     // MagickWand*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;

    char *ptr = (char *)MagickGetImageBlob((MagickWand *)(long)PARAM(0), &local_parameter_1);
    RETURN_BUFFER(ptr, local_parameter_1)
    if (ptr)
        MagickRelinquishMemory(ptr);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImagesBlob, 1)
    T_HANDLE(MagickGetImagesBlob, 0)     // MagickWand*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;

    char *ptr = (char *)MagickGetImagesBlob((MagickWand *)(long)PARAM(0), &local_parameter_1);
    RETURN_BUFFER(ptr, local_parameter_1)
    if (ptr)
        MagickRelinquishMemory(ptr);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageVirtualPixelMethod, 1)
    T_HANDLE(MagickGetImageVirtualPixelMethod, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetImageVirtualPixelMethod((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageVirtualPixelMethod, 2)
    T_HANDLE(MagickSetImageVirtualPixelMethod, 0)     // MagickWand*
    T_NUMBER(MagickSetImageVirtualPixelMethod, 1)     // VirtualPixelMethod

    RETURN_NUMBER(MagickSetImageVirtualPixelMethod((MagickWand *)(long)PARAM(0), (VirtualPixelMethod)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetTextAlignment, 1)
    T_HANDLE(DrawGetTextAlignment, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetTextAlignment((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetClipPath, 1)
    T_HANDLE(DrawGetClipPath, 0)     // DrawingWand*

    RETURN_STRING2((char *)DrawGetClipPath((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetException, 2)
    T_HANDLE(DrawGetException, 0)     // DrawingWand*

// ... parameter 1 is by reference (ExceptionType*)
    ExceptionType local_parameter_1;

    RETURN_STRING2((char *)DrawGetException((DrawingWand *)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetFont, 1)
    T_HANDLE(DrawGetFont, 0)     // DrawingWand*

    RETURN_STRING2((char *)DrawGetFont((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetFontFamily, 1)
    T_HANDLE(DrawGetFontFamily, 0)     // DrawingWand*

    RETURN_STRING2((char *)DrawGetFontFamily((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetTextEncoding, 1)
    T_HANDLE(DrawGetTextEncoding, 0)     // DrawingWand*

    RETURN_STRING2((char *)DrawGetTextEncoding((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetVectorGraphics, 1)
    T_HANDLE(DrawGetVectorGraphics, 0)     // DrawingWand*

    RETURN_STRING2((char *)DrawGetVectorGraphics((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetClipUnits, 1)
    T_HANDLE(DrawGetClipUnits, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetClipUnits((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetTextDecoration, 1)
    T_HANDLE(DrawGetTextDecoration, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetTextDecoration((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetFillOpacity, 1)
    T_HANDLE(DrawGetFillOpacity, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetFillOpacity((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetFontSize, 1)
    T_HANDLE(DrawGetFontSize, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetFontSize((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetOpacity, 1)
    T_HANDLE(DrawGetOpacity, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetOpacity((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetStrokeDashArray, 2)
    T_HANDLE(DrawGetStrokeDashArray, 0)     // DrawingWand*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;

    RETURN_NUMBER((long)DrawGetStrokeDashArray((DrawingWand *)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetStrokeDashOffset, 1)
    T_HANDLE(DrawGetStrokeDashOffset, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetStrokeDashOffset((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetStrokeOpacity, 1)
    T_HANDLE(DrawGetStrokeOpacity, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetStrokeOpacity((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetStrokeWidth, 1)
    T_HANDLE(DrawGetStrokeWidth, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetStrokeWidth((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetTextKerning, 1)
    T_HANDLE(DrawGetTextKerning, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetTextKerning((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetTextInterlineSpacing, 1)
    T_HANDLE(DrawGetTextInterlineSpacing, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetTextInterlineSpacing((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetTextInterwordSpacing, 1)
    T_HANDLE(DrawGetTextInterwordSpacing, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetTextInterwordSpacing((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PeekDrawingWand, 1)
    T_HANDLE(PeekDrawingWand, 0)     // DrawingWand*

    RETURN_NUMBER((long)PeekDrawingWand((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(CloneDrawingWand, 1)
    T_HANDLE(CloneDrawingWand, 0)     // DrawingWand*

    RETURN_NUMBER((long)CloneDrawingWand((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DestroyDrawingWand, 1)
    T_HANDLE(DestroyDrawingWand, 0)     // DrawingWand*

    RETURN_NUMBER((long)DestroyDrawingWand((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawAllocateWand, 2)
    T_NUMBER(DrawAllocateWand, 0)
    T_HANDLE(DrawAllocateWand, 1)     // Image*

    DrawInfo * local_parameter_0 = (DrawInfo *)PARAM_INT(0);

    RETURN_NUMBER((long)DrawAllocateWand(local_parameter_0, (Image *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(NewDrawingWand, 0)
    RETURN_NUMBER((long)NewDrawingWand())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetExceptionType, 1)
    T_HANDLE(DrawGetExceptionType, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetExceptionType((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetClipRule, 1)
    T_HANDLE(DrawGetClipRule, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetClipRule((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetFillRule, 1)
    T_HANDLE(DrawGetFillRule, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetFillRule((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetGravity, 1)
    T_HANDLE(DrawGetGravity, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetGravity((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetStrokeLineCap, 1)
    T_HANDLE(DrawGetStrokeLineCap, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetStrokeLineCap((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetStrokeLineJoin, 1)
    T_HANDLE(DrawGetStrokeLineJoin, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetStrokeLineJoin((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawClearException, 1)
    T_HANDLE(DrawClearException, 0)     // DrawingWand*

    RETURN_NUMBER(DrawClearException((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawComposite, 7)
    T_HANDLE(DrawComposite, 0)     // DrawingWand*
    T_NUMBER(DrawComposite, 1)     // CompositeOperator
    T_NUMBER(DrawComposite, 2)     // double
    T_NUMBER(DrawComposite, 3)     // double
    T_NUMBER(DrawComposite, 4)     // double
    T_NUMBER(DrawComposite, 5)     // double
    T_HANDLE(DrawComposite, 6)     // MagickWand*

    RETURN_NUMBER(DrawComposite((DrawingWand *)(long)PARAM(0), (CompositeOperator)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4), (double)PARAM(5), (MagickWand *)(long)PARAM(6)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetFontResolution, 3)
    T_HANDLE(DrawGetFontResolution, 0)     // DrawingWand*

// ... parameter 1 is by reference (double*)
    double local_parameter_1;
// ... parameter 2 is by reference (double*)
    double local_parameter_2;

    RETURN_NUMBER(DrawGetFontResolution((DrawingWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetStrokeAntialias, 1)
    T_HANDLE(DrawGetStrokeAntialias, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetStrokeAntialias((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetTextAntialias, 1)
    T_HANDLE(DrawGetTextAntialias, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetTextAntialias((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPopPattern, 1)
    T_HANDLE(DrawPopPattern, 0)     // DrawingWand*

    RETURN_NUMBER(DrawPopPattern((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPushPattern, 6)
    T_HANDLE(DrawPushPattern, 0)     // DrawingWand*
    T_STRING(DrawPushPattern, 1)     // char*
    T_NUMBER(DrawPushPattern, 2)     // double
    T_NUMBER(DrawPushPattern, 3)     // double
    T_NUMBER(DrawPushPattern, 4)     // double
    T_NUMBER(DrawPushPattern, 5)     // double

    RETURN_NUMBER(DrawPushPattern((DrawingWand *)(long)PARAM(0), (char *)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4), (double)PARAM(5)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawRender, 1)
    T_HANDLE(DrawRender, 0)     // DrawingWand*

    RETURN_NUMBER(DrawRender((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetClipPath, 2)
    T_HANDLE(DrawSetClipPath, 0)     // DrawingWand*
    T_STRING(DrawSetClipPath, 1)     // char*

    RETURN_NUMBER(DrawSetClipPath((DrawingWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetFillPatternURL, 2)
    T_HANDLE(DrawSetFillPatternURL, 0)     // DrawingWand*
    T_STRING(DrawSetFillPatternURL, 1)     // char*

    RETURN_NUMBER(DrawSetFillPatternURL((DrawingWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetFont, 2)
    T_HANDLE(DrawSetFont, 0)     // DrawingWand*
    T_STRING(DrawSetFont, 1)     // char*

    RETURN_NUMBER(DrawSetFont((DrawingWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetFontFamily, 2)
    T_HANDLE(DrawSetFontFamily, 0)     // DrawingWand*
    T_STRING(DrawSetFontFamily, 1)     // char*

    RETURN_NUMBER(DrawSetFontFamily((DrawingWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetFontResolution, 3)
    T_HANDLE(DrawSetFontResolution, 0)     // DrawingWand*
    T_NUMBER(DrawSetFontResolution, 1)     // double
    T_NUMBER(DrawSetFontResolution, 2)     // double

    RETURN_NUMBER(DrawSetFontResolution((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetStrokeDashArray, 3)
    T_HANDLE(DrawSetStrokeDashArray, 0)     // DrawingWand*
    T_NUMBER(DrawSetStrokeDashArray, 1)     // size_t

// ... parameter 2 is by reference (double*)
    double local_parameter_2;

    RETURN_NUMBER(DrawSetStrokeDashArray((DrawingWand *)(long)PARAM(0), (size_t)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetStrokePatternURL, 2)
    T_HANDLE(DrawSetStrokePatternURL, 0)     // DrawingWand*
    T_STRING(DrawSetStrokePatternURL, 1)     // char*

    RETURN_NUMBER(DrawSetStrokePatternURL((DrawingWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetVectorGraphics, 2)
    T_HANDLE(DrawSetVectorGraphics, 0)     // DrawingWand*
    T_STRING(DrawSetVectorGraphics, 1)     // char*

    RETURN_NUMBER(DrawSetVectorGraphics((DrawingWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(IsDrawingWand, 1)
    T_HANDLE(IsDrawingWand, 0)     // DrawingWand*

    RETURN_NUMBER(IsDrawingWand((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PopDrawingWand, 1)
    T_HANDLE(PopDrawingWand, 0)     // DrawingWand*

    RETURN_NUMBER(PopDrawingWand((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PushDrawingWand, 1)
    T_HANDLE(PushDrawingWand, 0)     // DrawingWand*

    RETURN_NUMBER(PushDrawingWand((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetFontStretch, 1)
    T_HANDLE(DrawGetFontStretch, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetFontStretch((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetFontStyle, 1)
    T_HANDLE(DrawGetFontStyle, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetFontStyle((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetFontWeight, 1)
    T_HANDLE(DrawGetFontWeight, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetFontWeight((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetStrokeMiterLimit, 1)
    T_HANDLE(DrawGetStrokeMiterLimit, 0)     // DrawingWand*

    RETURN_NUMBER(DrawGetStrokeMiterLimit((DrawingWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ClearDrawingWand, 1)
    T_HANDLE(ClearDrawingWand, 0)     // DrawingWand*

    ClearDrawingWand((DrawingWand *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawAffine, 7)
    T_HANDLE(DrawAffine, 0)     // DrawingWand*
    T_NUMBER(DrawAffine, 1)
    T_NUMBER(DrawAffine, 2)
    T_NUMBER(DrawAffine, 3)
    T_NUMBER(DrawAffine, 4)
    T_NUMBER(DrawAffine, 5)
    T_NUMBER(DrawAffine, 6)

    AffineMatrix local_parameter_1;
    local_parameter_1.sx = PARAM(1);
    local_parameter_1.rx = PARAM(2);
    local_parameter_1.ry = PARAM(3);
    local_parameter_1.sy = PARAM(4);
    local_parameter_1.tx = PARAM(5);
    local_parameter_1.ty = PARAM(6);

    DrawAffine((DrawingWand *)(long)PARAM(0), &local_parameter_1);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawAnnotation, 4)
    T_HANDLE(DrawAnnotation, 0)     // DrawingWand*
    T_NUMBER(DrawAnnotation, 1)     // double
    T_NUMBER(DrawAnnotation, 2)     // double
    T_STRING(DrawAnnotation, 3)     // unsigned

    DrawAnnotation((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (unsigned char *)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawArc, 7)
    T_HANDLE(DrawArc, 0)     // DrawingWand*
    T_NUMBER(DrawArc, 1)     // double
    T_NUMBER(DrawArc, 2)     // double
    T_NUMBER(DrawArc, 3)     // double
    T_NUMBER(DrawArc, 4)     // double
    T_NUMBER(DrawArc, 5)     // double
    T_NUMBER(DrawArc, 6)     // double

    DrawArc((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4), (double)PARAM(5), (double)PARAM(6));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawBezier, 2)
    T_HANDLE(DrawBezier, 0)     // DrawingWand*
    T_ARRAY(DrawBezier, 1)

    void *arr = PARAMETER(1);
    int count = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
    if (count > 0) {
        PointInfo *local_parameter_2 = new PointInfo[count];
        for (int i = 0; i < count; i++) {
            void *newpData;
            Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_ARRAY) {
                    void *newpData2 = 0;
                    Invoke(INVOKE_ARRAY_VARIABLE, newpData, 0, &newpData2);
                    if (newpData2) {
                        Invoke(INVOKE_GET_VARIABLE, newpData2, &type, &szData, &nData);
                        if (type == VARIABLE_NUMBER) {
                            local_parameter_2[i].x = nData;
                        }
                    }
                    Invoke(INVOKE_ARRAY_VARIABLE, newpData, 1, &newpData2);
                    if (newpData2) {
                        Invoke(INVOKE_GET_VARIABLE, newpData2, &type, &szData, &nData);
                        if (type == VARIABLE_NUMBER) {
                            local_parameter_2[i].y = nData;
                        }
                    }
                }
            }
        }
        DrawBezier((DrawingWand *)(long)PARAM(0), count, local_parameter_2);
        delete[] local_parameter_2;
    }

    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetBorderColor, 2)
    T_HANDLE(DrawGetBorderColor, 0)     // DrawingWand*
    T_HANDLE(DrawGetBorderColor, 1)     // PixelWand*

    DrawGetBorderColor((DrawingWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawCircle, 5)
    T_HANDLE(DrawCircle, 0)     // DrawingWand*
    T_NUMBER(DrawCircle, 1)     // double
    T_NUMBER(DrawCircle, 2)     // double
    T_NUMBER(DrawCircle, 3)     // double
    T_NUMBER(DrawCircle, 4)     // double

    DrawCircle((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawColor, 4)
    T_HANDLE(DrawColor, 0)     // DrawingWand*
    T_NUMBER(DrawColor, 1)     // double
    T_NUMBER(DrawColor, 2)     // double
    T_NUMBER(DrawColor, 3)     // PaintMethod

    DrawColor((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (PaintMethod)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawComment, 2)
    T_HANDLE(DrawComment, 0)     // DrawingWand*
    T_STRING(DrawComment, 1)     // char*

    DrawComment((DrawingWand *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawEllipse, 7)
    T_HANDLE(DrawEllipse, 0)     // DrawingWand*
    T_NUMBER(DrawEllipse, 1)     // double
    T_NUMBER(DrawEllipse, 2)     // double
    T_NUMBER(DrawEllipse, 3)     // double
    T_NUMBER(DrawEllipse, 4)     // double
    T_NUMBER(DrawEllipse, 5)     // double
    T_NUMBER(DrawEllipse, 6)     // double

    DrawEllipse((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4), (double)PARAM(5), (double)PARAM(6));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetFillColor, 2)
    T_HANDLE(DrawGetFillColor, 0)     // DrawingWand*
    T_HANDLE(DrawGetFillColor, 1)     // PixelWand*

    DrawGetFillColor((DrawingWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetStrokeColor, 2)
    T_HANDLE(DrawGetStrokeColor, 0)     // DrawingWand*
    T_HANDLE(DrawGetStrokeColor, 1)     // PixelWand*

    DrawGetStrokeColor((DrawingWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetTextKerning, 2)
    T_HANDLE(DrawSetTextKerning, 0)     // DrawingWand*
    T_NUMBER(DrawSetTextKerning, 1)     // double

    DrawSetTextKerning((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetTextInterlineSpacing, 2)
    T_HANDLE(DrawSetTextInterlineSpacing, 0)     // DrawingWand*
    T_NUMBER(DrawSetTextInterlineSpacing, 1)     // double

    DrawSetTextInterlineSpacing((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetTextInterwordSpacing, 2)
    T_HANDLE(DrawSetTextInterwordSpacing, 0)     // DrawingWand*
    T_NUMBER(DrawSetTextInterwordSpacing, 1)     // double

    DrawSetTextInterwordSpacing((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawGetTextUnderColor, 2)
    T_HANDLE(DrawGetTextUnderColor, 0)     // DrawingWand*
    T_HANDLE(DrawGetTextUnderColor, 1)     // PixelWand*

    DrawGetTextUnderColor((DrawingWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawLine, 5)
    T_HANDLE(DrawLine, 0)     // DrawingWand*
    T_NUMBER(DrawLine, 1)     // double
    T_NUMBER(DrawLine, 2)     // double
    T_NUMBER(DrawLine, 3)     // double
    T_NUMBER(DrawLine, 4)     // double

    DrawLine((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawMatte, 4)
    T_HANDLE(DrawMatte, 0)     // DrawingWand*
    T_NUMBER(DrawMatte, 1)     // double
    T_NUMBER(DrawMatte, 2)     // double
    T_NUMBER(DrawMatte, 3)     // PaintMethod

    DrawMatte((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (PaintMethod)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathClose, 1)
    T_HANDLE(DrawPathClose, 0)     // DrawingWand*

    DrawPathClose((DrawingWand *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathCurveToAbsolute, 7)
    T_HANDLE(DrawPathCurveToAbsolute, 0)     // DrawingWand*
    T_NUMBER(DrawPathCurveToAbsolute, 1)     // double
    T_NUMBER(DrawPathCurveToAbsolute, 2)     // double
    T_NUMBER(DrawPathCurveToAbsolute, 3)     // double
    T_NUMBER(DrawPathCurveToAbsolute, 4)     // double
    T_NUMBER(DrawPathCurveToAbsolute, 5)     // double
    T_NUMBER(DrawPathCurveToAbsolute, 6)     // double

    DrawPathCurveToAbsolute((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4), (double)PARAM(5), (double)PARAM(6));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathCurveToRelative, 7)
    T_HANDLE(DrawPathCurveToRelative, 0)     // DrawingWand*
    T_NUMBER(DrawPathCurveToRelative, 1)     // double
    T_NUMBER(DrawPathCurveToRelative, 2)     // double
    T_NUMBER(DrawPathCurveToRelative, 3)     // double
    T_NUMBER(DrawPathCurveToRelative, 4)     // double
    T_NUMBER(DrawPathCurveToRelative, 5)     // double
    T_NUMBER(DrawPathCurveToRelative, 6)     // double

    DrawPathCurveToRelative((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4), (double)PARAM(5), (double)PARAM(6));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathCurveToQuadraticBezierAbsolute, 5)
    T_HANDLE(DrawPathCurveToQuadraticBezierAbsolute, 0)     // DrawingWand*
    T_NUMBER(DrawPathCurveToQuadraticBezierAbsolute, 1)     // double
    T_NUMBER(DrawPathCurveToQuadraticBezierAbsolute, 2)     // double
    T_NUMBER(DrawPathCurveToQuadraticBezierAbsolute, 3)     // double
    T_NUMBER(DrawPathCurveToQuadraticBezierAbsolute, 4)     // double

    DrawPathCurveToQuadraticBezierAbsolute((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathCurveToQuadraticBezierRelative, 5)
    T_HANDLE(DrawPathCurveToQuadraticBezierRelative, 0)     // DrawingWand*
    T_NUMBER(DrawPathCurveToQuadraticBezierRelative, 1)     // double
    T_NUMBER(DrawPathCurveToQuadraticBezierRelative, 2)     // double
    T_NUMBER(DrawPathCurveToQuadraticBezierRelative, 3)     // double
    T_NUMBER(DrawPathCurveToQuadraticBezierRelative, 4)     // double

    DrawPathCurveToQuadraticBezierRelative((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathCurveToQuadraticBezierSmoothAbsolute, 3)
    T_HANDLE(DrawPathCurveToQuadraticBezierSmoothAbsolute, 0)     // DrawingWand*
    T_NUMBER(DrawPathCurveToQuadraticBezierSmoothAbsolute, 1)     // double
    T_NUMBER(DrawPathCurveToQuadraticBezierSmoothAbsolute, 2)     // double

    DrawPathCurveToQuadraticBezierSmoothAbsolute((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathCurveToQuadraticBezierSmoothRelative, 3)
    T_HANDLE(DrawPathCurveToQuadraticBezierSmoothRelative, 0)     // DrawingWand*
    T_NUMBER(DrawPathCurveToQuadraticBezierSmoothRelative, 1)     // double
    T_NUMBER(DrawPathCurveToQuadraticBezierSmoothRelative, 2)     // double

    DrawPathCurveToQuadraticBezierSmoothRelative((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathCurveToSmoothAbsolute, 5)
    T_HANDLE(DrawPathCurveToSmoothAbsolute, 0)     // DrawingWand*
    T_NUMBER(DrawPathCurveToSmoothAbsolute, 1)     // double
    T_NUMBER(DrawPathCurveToSmoothAbsolute, 2)     // double
    T_NUMBER(DrawPathCurveToSmoothAbsolute, 3)     // double
    T_NUMBER(DrawPathCurveToSmoothAbsolute, 4)     // double

    DrawPathCurveToSmoothAbsolute((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathCurveToSmoothRelative, 5)
    T_HANDLE(DrawPathCurveToSmoothRelative, 0)     // DrawingWand*
    T_NUMBER(DrawPathCurveToSmoothRelative, 1)     // double
    T_NUMBER(DrawPathCurveToSmoothRelative, 2)     // double
    T_NUMBER(DrawPathCurveToSmoothRelative, 3)     // double
    T_NUMBER(DrawPathCurveToSmoothRelative, 4)     // double

    DrawPathCurveToSmoothRelative((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathEllipticArcAbsolute, 8)
    T_HANDLE(DrawPathEllipticArcAbsolute, 0)     // DrawingWand*
    T_NUMBER(DrawPathEllipticArcAbsolute, 1)     // double
    T_NUMBER(DrawPathEllipticArcAbsolute, 2)     // double
    T_NUMBER(DrawPathEllipticArcAbsolute, 3)     // double
    T_NUMBER(DrawPathEllipticArcAbsolute, 4)     // MagickBooleanType
    T_NUMBER(DrawPathEllipticArcAbsolute, 5)     // MagickBooleanType
    T_NUMBER(DrawPathEllipticArcAbsolute, 6)     // double
    T_NUMBER(DrawPathEllipticArcAbsolute, 7)     // double

    DrawPathEllipticArcAbsolute((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (MagickBooleanType)PARAM(4), (MagickBooleanType)PARAM(5), (double)PARAM(6), (double)PARAM(7));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathEllipticArcRelative, 8)
    T_HANDLE(DrawPathEllipticArcRelative, 0)     // DrawingWand*
    T_NUMBER(DrawPathEllipticArcRelative, 1)     // double
    T_NUMBER(DrawPathEllipticArcRelative, 2)     // double
    T_NUMBER(DrawPathEllipticArcRelative, 3)     // double
    T_NUMBER(DrawPathEllipticArcRelative, 4)     // MagickBooleanType
    T_NUMBER(DrawPathEllipticArcRelative, 5)     // MagickBooleanType
    T_NUMBER(DrawPathEllipticArcRelative, 6)     // double
    T_NUMBER(DrawPathEllipticArcRelative, 7)     // double

    DrawPathEllipticArcRelative((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (MagickBooleanType)PARAM(4), (MagickBooleanType)PARAM(5), (double)PARAM(6), (double)PARAM(7));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathFinish, 1)
    T_HANDLE(DrawPathFinish, 0)     // DrawingWand*

    DrawPathFinish((DrawingWand *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathLineToAbsolute, 3)
    T_HANDLE(DrawPathLineToAbsolute, 0)     // DrawingWand*
    T_NUMBER(DrawPathLineToAbsolute, 1)     // double
    T_NUMBER(DrawPathLineToAbsolute, 2)     // double

    DrawPathLineToAbsolute((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathLineToRelative, 3)
    T_HANDLE(DrawPathLineToRelative, 0)     // DrawingWand*
    T_NUMBER(DrawPathLineToRelative, 1)     // double
    T_NUMBER(DrawPathLineToRelative, 2)     // double

    DrawPathLineToRelative((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathLineToHorizontalAbsolute, 2)
    T_HANDLE(DrawPathLineToHorizontalAbsolute, 0)     // DrawingWand*
    T_NUMBER(DrawPathLineToHorizontalAbsolute, 1)     // double

    DrawPathLineToHorizontalAbsolute((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathLineToHorizontalRelative, 2)
    T_HANDLE(DrawPathLineToHorizontalRelative, 0)     // DrawingWand*
    T_NUMBER(DrawPathLineToHorizontalRelative, 1)     // double

    DrawPathLineToHorizontalRelative((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathLineToVerticalAbsolute, 2)
    T_HANDLE(DrawPathLineToVerticalAbsolute, 0)     // DrawingWand*
    T_NUMBER(DrawPathLineToVerticalAbsolute, 1)     // double

    DrawPathLineToVerticalAbsolute((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathLineToVerticalRelative, 2)
    T_HANDLE(DrawPathLineToVerticalRelative, 0)     // DrawingWand*
    T_NUMBER(DrawPathLineToVerticalRelative, 1)     // double

    DrawPathLineToVerticalRelative((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathMoveToAbsolute, 3)
    T_HANDLE(DrawPathMoveToAbsolute, 0)     // DrawingWand*
    T_NUMBER(DrawPathMoveToAbsolute, 1)     // double
    T_NUMBER(DrawPathMoveToAbsolute, 2)     // double

    DrawPathMoveToAbsolute((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathMoveToRelative, 3)
    T_HANDLE(DrawPathMoveToRelative, 0)     // DrawingWand*
    T_NUMBER(DrawPathMoveToRelative, 1)     // double
    T_NUMBER(DrawPathMoveToRelative, 2)     // double

    DrawPathMoveToRelative((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPathStart, 1)
    T_HANDLE(DrawPathStart, 0)     // DrawingWand*

    DrawPathStart((DrawingWand *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPoint, 3)
    T_HANDLE(DrawPoint, 0)     // DrawingWand*
    T_NUMBER(DrawPoint, 1)     // double
    T_NUMBER(DrawPoint, 2)     // double

    DrawPoint((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPolygon, 2)
    T_HANDLE(DrawPolygon, 0)     // DrawingWand*
    T_ARRAY(DrawPolygon, 1)

    void *arr = PARAMETER(1);
    int count = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
    if (count > 0) {
        PointInfo *local_parameter_2 = new PointInfo[count];
        for (int i = 0; i < count; i++) {
            void *newpData;
            Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_ARRAY) {
                    void *newpData2 = 0;
                    Invoke(INVOKE_ARRAY_VARIABLE, newpData, 0, &newpData2);
                    if (newpData2) {
                        Invoke(INVOKE_GET_VARIABLE, newpData2, &type, &szData, &nData);
                        if (type == VARIABLE_NUMBER) {
                            local_parameter_2[i].x = nData;
                        }
                    }
                    Invoke(INVOKE_ARRAY_VARIABLE, newpData, 1, &newpData2);
                    if (newpData2) {
                        Invoke(INVOKE_GET_VARIABLE, newpData2, &type, &szData, &nData);
                        if (type == VARIABLE_NUMBER) {
                            local_parameter_2[i].y = nData;
                        }
                    }
                }
            }
        }
        DrawPolygon((DrawingWand *)(long)PARAM(0), count, local_parameter_2);
        delete[] local_parameter_2;
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPolyline, 2)
    T_HANDLE(DrawPolyline, 0)     // DrawingWand*
    T_ARRAY(DrawPolyline, 1)

    void *arr = PARAMETER(1);
    int count = Invoke(INVOKE_GET_ARRAY_COUNT, arr);
    if (count > 0) {
        PointInfo *local_parameter_2 = new PointInfo[count];
        for (int i = 0; i < count; i++) {
            void *newpData;
            Invoke(INVOKE_ARRAY_VARIABLE, arr, i, &newpData);
            if (newpData) {
                char    *szData;
                INTEGER type;
                NUMBER  nData;

                Invoke(INVOKE_GET_VARIABLE, newpData, &type, &szData, &nData);
                if (type == VARIABLE_ARRAY) {
                    void *newpData2 = 0;
                    Invoke(INVOKE_ARRAY_VARIABLE, newpData, 0, &newpData2);
                    if (newpData2) {
                        Invoke(INVOKE_GET_VARIABLE, newpData2, &type, &szData, &nData);
                        if (type == VARIABLE_NUMBER) {
                            local_parameter_2[i].x = nData;
                        }
                    }
                    Invoke(INVOKE_ARRAY_VARIABLE, newpData, 1, &newpData2);
                    if (newpData2) {
                        Invoke(INVOKE_GET_VARIABLE, newpData2, &type, &szData, &nData);
                        if (type == VARIABLE_NUMBER) {
                            local_parameter_2[i].y = nData;
                        }
                    }
                }
            }
        }
        DrawPolyline((DrawingWand *)(long)PARAM(0), count, local_parameter_2);
        delete[] local_parameter_2;
    }
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPopClipPath, 1)
    T_HANDLE(DrawPopClipPath, 0)     // DrawingWand*

    DrawPopClipPath((DrawingWand *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPopDefs, 1)
    T_HANDLE(DrawPopDefs, 0)     // DrawingWand*

    DrawPopDefs((DrawingWand *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPushClipPath, 2)
    T_HANDLE(DrawPushClipPath, 0)     // DrawingWand*
    T_STRING(DrawPushClipPath, 1)     // char*

    DrawPushClipPath((DrawingWand *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawPushDefs, 1)
    T_HANDLE(DrawPushDefs, 0)     // DrawingWand*

    DrawPushDefs((DrawingWand *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawRectangle, 5)
    T_HANDLE(DrawRectangle, 0)     // DrawingWand*
    T_NUMBER(DrawRectangle, 1)     // double
    T_NUMBER(DrawRectangle, 2)     // double
    T_NUMBER(DrawRectangle, 3)     // double
    T_NUMBER(DrawRectangle, 4)     // double

    DrawRectangle((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawResetVectorGraphics, 1)
    T_HANDLE(DrawResetVectorGraphics, 0)     // DrawingWand*

    DrawResetVectorGraphics((DrawingWand *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawRotate, 2)
    T_HANDLE(DrawRotate, 0)     // DrawingWand*
    T_NUMBER(DrawRotate, 1)     // double

    DrawRotate((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawRoundRectangle, 7)
    T_HANDLE(DrawRoundRectangle, 0)     // DrawingWand*
    T_NUMBER(DrawRoundRectangle, 1)     // double
    T_NUMBER(DrawRoundRectangle, 2)     // double
    T_NUMBER(DrawRoundRectangle, 3)     // double
    T_NUMBER(DrawRoundRectangle, 4)     // double
    T_NUMBER(DrawRoundRectangle, 5)     // double
    T_NUMBER(DrawRoundRectangle, 6)     // double

    DrawRoundRectangle((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3), (double)PARAM(4), (double)PARAM(5), (double)PARAM(6));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawScale, 3)
    T_HANDLE(DrawScale, 0)     // DrawingWand*
    T_NUMBER(DrawScale, 1)     // double
    T_NUMBER(DrawScale, 2)     // double

    DrawScale((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetBorderColor, 2)
    T_HANDLE(DrawSetBorderColor, 0)     // DrawingWand*
    T_HANDLE(DrawSetBorderColor, 1)     // PixelWand*

    DrawSetBorderColor((DrawingWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetClipRule, 2)
    T_HANDLE(DrawSetClipRule, 0)     // DrawingWand*
    T_NUMBER(DrawSetClipRule, 1)     // FillRule

    DrawSetClipRule((DrawingWand *)(long)PARAM(0), (FillRule)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetClipUnits, 2)
    T_HANDLE(DrawSetClipUnits, 0)     // DrawingWand*
    T_NUMBER(DrawSetClipUnits, 1)     // ClipPathUnits

    DrawSetClipUnits((DrawingWand *)(long)PARAM(0), (ClipPathUnits)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetFillColor, 2)
    T_HANDLE(DrawSetFillColor, 0)     // DrawingWand*
    T_HANDLE(DrawSetFillColor, 1)     // PixelWand*

    DrawSetFillColor((DrawingWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetFillOpacity, 2)
    T_HANDLE(DrawSetFillOpacity, 0)     // DrawingWand*
    T_NUMBER(DrawSetFillOpacity, 1)     // double

    DrawSetFillOpacity((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetFillRule, 2)
    T_HANDLE(DrawSetFillRule, 0)     // DrawingWand*
    T_NUMBER(DrawSetFillRule, 1)     // FillRule

    DrawSetFillRule((DrawingWand *)(long)PARAM(0), (FillRule)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetFontSize, 2)
    T_HANDLE(DrawSetFontSize, 0)     // DrawingWand*
    T_NUMBER(DrawSetFontSize, 1)     // double

    DrawSetFontSize((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetFontStretch, 2)
    T_HANDLE(DrawSetFontStretch, 0)     // DrawingWand*
    T_NUMBER(DrawSetFontStretch, 1)     // StretchType

    DrawSetFontStretch((DrawingWand *)(long)PARAM(0), (StretchType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetFontStyle, 2)
    T_HANDLE(DrawSetFontStyle, 0)     // DrawingWand*
    T_NUMBER(DrawSetFontStyle, 1)     // StyleType

    DrawSetFontStyle((DrawingWand *)(long)PARAM(0), (StyleType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetFontWeight, 2)
    T_HANDLE(DrawSetFontWeight, 0)     // DrawingWand*
    T_NUMBER(DrawSetFontWeight, 1)     // size_t

    DrawSetFontWeight((DrawingWand *)(long)PARAM(0), (size_t)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetGravity, 2)
    T_HANDLE(DrawSetGravity, 0)     // DrawingWand*
    T_NUMBER(DrawSetGravity, 1)     // GravityType

    DrawSetGravity((DrawingWand *)(long)PARAM(0), (GravityType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetOpacity, 2)
    T_HANDLE(DrawSetOpacity, 0)     // DrawingWand*
    T_NUMBER(DrawSetOpacity, 1)     // double

    DrawSetOpacity((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetStrokeAntialias, 2)
    T_HANDLE(DrawSetStrokeAntialias, 0)     // DrawingWand*
    T_NUMBER(DrawSetStrokeAntialias, 1)     // MagickBooleanType

    DrawSetStrokeAntialias((DrawingWand *)(long)PARAM(0), (MagickBooleanType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetStrokeColor, 2)
    T_HANDLE(DrawSetStrokeColor, 0)     // DrawingWand*
    T_HANDLE(DrawSetStrokeColor, 1)     // PixelWand*

    DrawSetStrokeColor((DrawingWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetStrokeDashOffset, 2)
    T_HANDLE(DrawSetStrokeDashOffset, 0)     // DrawingWand*
    T_NUMBER(DrawSetStrokeDashOffset, 1)     // double

    DrawSetStrokeDashOffset((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetStrokeLineCap, 2)
    T_HANDLE(DrawSetStrokeLineCap, 0)     // DrawingWand*
    T_NUMBER(DrawSetStrokeLineCap, 1)     // LineCap

    DrawSetStrokeLineCap((DrawingWand *)(long)PARAM(0), (LineCap)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetStrokeLineJoin, 2)
    T_HANDLE(DrawSetStrokeLineJoin, 0)     // DrawingWand*
    T_NUMBER(DrawSetStrokeLineJoin, 1)     // LineJoin

    DrawSetStrokeLineJoin((DrawingWand *)(long)PARAM(0), (LineJoin)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetStrokeMiterLimit, 2)
    T_HANDLE(DrawSetStrokeMiterLimit, 0)     // DrawingWand*
    T_NUMBER(DrawSetStrokeMiterLimit, 1)     // size_t

    DrawSetStrokeMiterLimit((DrawingWand *)(long)PARAM(0), (size_t)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetStrokeOpacity, 2)
    T_HANDLE(DrawSetStrokeOpacity, 0)     // DrawingWand*
    T_NUMBER(DrawSetStrokeOpacity, 1)     // double

    DrawSetStrokeOpacity((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetStrokeWidth, 2)
    T_HANDLE(DrawSetStrokeWidth, 0)     // DrawingWand*
    T_NUMBER(DrawSetStrokeWidth, 1)     // double

    DrawSetStrokeWidth((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetTextAlignment, 2)
    T_HANDLE(DrawSetTextAlignment, 0)     // DrawingWand*
    T_NUMBER(DrawSetTextAlignment, 1)     // AlignType

    DrawSetTextAlignment((DrawingWand *)(long)PARAM(0), (AlignType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetTextAntialias, 2)
    T_HANDLE(DrawSetTextAntialias, 0)     // DrawingWand*
    T_NUMBER(DrawSetTextAntialias, 1)     // MagickBooleanType

    DrawSetTextAntialias((DrawingWand *)(long)PARAM(0), (MagickBooleanType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetTextDecoration, 2)
    T_HANDLE(DrawSetTextDecoration, 0)     // DrawingWand*
    T_NUMBER(DrawSetTextDecoration, 1)     // DecorationType

    DrawSetTextDecoration((DrawingWand *)(long)PARAM(0), (DecorationType)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetTextEncoding, 2)
    T_HANDLE(DrawSetTextEncoding, 0)     // DrawingWand*
    T_STRING(DrawSetTextEncoding, 1)     // char*

    DrawSetTextEncoding((DrawingWand *)(long)PARAM(0), (char *)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetTextUnderColor, 2)
    T_HANDLE(DrawSetTextUnderColor, 0)     // DrawingWand*
    T_HANDLE(DrawSetTextUnderColor, 1)     // PixelWand*

    DrawSetTextUnderColor((DrawingWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSetViewbox, 5)
    T_HANDLE(DrawSetViewbox, 0)     // DrawingWand*
    T_NUMBER(DrawSetViewbox, 1)     // ssize_t
    T_NUMBER(DrawSetViewbox, 2)     // ssize_t
    T_NUMBER(DrawSetViewbox, 3)     // ssize_t
    T_NUMBER(DrawSetViewbox, 4)     // ssize_t

    DrawSetViewbox((DrawingWand *)(long)PARAM(0), (ssize_t)PARAM(1), (ssize_t)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSkewX, 2)
    T_HANDLE(DrawSkewX, 0)     // DrawingWand*
    T_NUMBER(DrawSkewX, 1)     // double

    DrawSkewX((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawSkewY, 2)
    T_HANDLE(DrawSkewY, 0)     // DrawingWand*
    T_NUMBER(DrawSkewY, 1)     // double

    DrawSkewY((DrawingWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DrawTranslate, 3)
    T_HANDLE(DrawTranslate, 0)     // DrawingWand*
    T_NUMBER(DrawTranslate, 1)     // double
    T_NUMBER(DrawTranslate, 2)     // double

    DrawTranslate((DrawingWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetColorAsNormalizedString, 1)
    T_HANDLE(PixelGetColorAsNormalizedString, 0)     // PixelWand*

    RETURN_STRING2((char *)PixelGetColorAsNormalizedString((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetColorAsString, 1)
    T_HANDLE(PixelGetColorAsString, 0)     // PixelWand*

    RETURN_STRING2((char *)PixelGetColorAsString((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetException, 2)
    T_HANDLE(PixelGetException, 0)     // PixelWand*

// ... parameter 1 is by reference (ExceptionType*)
    ExceptionType local_parameter_1;

    RETURN_STRING2((char *)PixelGetException((PixelWand *)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetAlpha, 1)
    T_HANDLE(PixelGetAlpha, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetAlpha((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetBlack, 1)
    T_HANDLE(PixelGetBlack, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetBlack((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetBlue, 1)
    T_HANDLE(PixelGetBlue, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetBlue((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetCyan, 1)
    T_HANDLE(PixelGetCyan, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetCyan((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetFuzz, 1)
    T_HANDLE(PixelGetFuzz, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetFuzz((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetGreen, 1)
    T_HANDLE(PixelGetGreen, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetGreen((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetMagenta, 1)
    T_HANDLE(PixelGetMagenta, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetMagenta((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetOpacity, 1)
    T_HANDLE(PixelGetOpacity, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetOpacity((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetRed, 1)
    T_HANDLE(PixelGetRed, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetRed((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetYellow, 1)
    T_HANDLE(PixelGetYellow, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetYellow((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetExceptionType, 1)
    T_HANDLE(PixelGetExceptionType, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetExceptionType((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetIndex, 1)
    T_HANDLE(PixelGetIndex, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetIndex((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(IsPixelWand, 1)
    T_HANDLE(IsPixelWand, 0)     // PixelWand*

    RETURN_NUMBER(IsPixelWand((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(IsPixelWandSimilar, 3)
    T_HANDLE(IsPixelWandSimilar, 0)     // PixelWand*
    T_HANDLE(IsPixelWandSimilar, 1)     // PixelWand*
    T_NUMBER(IsPixelWandSimilar, 2)     // double

    RETURN_NUMBER(IsPixelWandSimilar((PixelWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelClearException, 1)
    T_HANDLE(PixelClearException, 0)     // PixelWand*

    RETURN_NUMBER(PixelClearException((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetColor, 2)
    T_HANDLE(PixelSetColor, 0)     // PixelWand*
    T_STRING(PixelSetColor, 1)     // char*

    RETURN_NUMBER(PixelSetColor((PixelWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ClonePixelWand, 1)
    T_HANDLE(ClonePixelWand, 0)     // PixelWand*

    RETURN_NUMBER((long)ClonePixelWand((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(ClonePixelWands,2)
        T_NUMBER(ClonePixelWand, 1) // size_t

        // ... parameter 0 is by reference (PixelWand**)
        PixelWand* local_parameter_0;

        RETURN_NUMBER((long)*ClonePixelWands(&local_parameter_0, (size_t)PARAM(1)))
        SET_NUMBER(0, (long)local_parameter_0)
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DestroyPixelWand, 1)
    T_HANDLE(DestroyPixelWand, 0)     // PixelWand*

    RETURN_NUMBER((long)DestroyPixelWand((PixelWand *)(long)PARAM(0)))
    SET_NUMBER(0, 0)
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(DestroyPixelWands,2)
        T_NUMBER(DestroyPixelWand, 1) // size_t

        // ... parameter 0 is by reference (PixelWand**)
        PixelWand* local_parameter_0;

        RETURN_NUMBER((long)*DestroyPixelWands(&local_parameter_0, (size_t)PARAM(1)))
        SET_NUMBER(0, 0)
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(NewPixelWand, 0)
    RETURN_NUMBER((long)NewPixelWand())
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(NewPixelWands,1)
        T_NUMBER(NewPixelWand, 0) // size_t

        RETURN_NUMBER((long)NewPixelWands((size_t)PARAM(0)))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetAlphaQuantum, 1)
    T_HANDLE(PixelGetAlphaQuantum, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetAlphaQuantum((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetBlackQuantum, 1)
    T_HANDLE(PixelGetBlackQuantum, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetBlackQuantum((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetBlueQuantum, 1)
    T_HANDLE(PixelGetBlueQuantum, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetBlueQuantum((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetCyanQuantum, 1)
    T_HANDLE(PixelGetCyanQuantum, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetCyanQuantum((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetGreenQuantum, 1)
    T_HANDLE(PixelGetGreenQuantum, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetGreenQuantum((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetMagentaQuantum, 1)
    T_HANDLE(PixelGetMagentaQuantum, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetMagentaQuantum((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetOpacityQuantum, 1)
    T_HANDLE(PixelGetOpacityQuantum, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetOpacityQuantum((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetRedQuantum, 1)
    T_HANDLE(PixelGetRedQuantum, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetRedQuantum((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetYellowQuantum, 1)
    T_HANDLE(PixelGetYellowQuantum, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetYellowQuantum((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetColorCount, 1)
    T_HANDLE(PixelGetColorCount, 0)     // PixelWand*

    RETURN_NUMBER(PixelGetColorCount((PixelWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ClearPixelWand, 1)
    T_HANDLE(ClearPixelWand, 0)     // PixelWand*

    ClearPixelWand((PixelWand *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetHSL, 4)
    T_HANDLE(PixelGetHSL, 0)     // PixelWand*

// ... parameter 1 is by reference (double*)
    double local_parameter_1;
// ... parameter 2 is by reference (double*)
    double local_parameter_2;
// ... parameter 3 is by reference (double*)
    double local_parameter_3;

    PixelGetHSL((PixelWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2, &local_parameter_3);
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetQuantumColor, 5)
    T_HANDLE(PixelGetQuantumColor, 0)     // PixelWand*

    PixelPacket local_parameter_1;

    PixelGetQuantumColor((PixelWand *)(long)PARAM(0), &local_parameter_1);
    SET_NUMBER(1, local_parameter_1.red)
    SET_NUMBER(2, local_parameter_1.green)
    SET_NUMBER(3, local_parameter_1.blue)
    SET_NUMBER(4, local_parameter_1.opacity)
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetAlpha, 2)
    T_HANDLE(PixelSetAlpha, 0)     // PixelWand*
    T_NUMBER(PixelSetAlpha, 1)     // double

    PixelSetAlpha((PixelWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetAlphaQuantum, 2)
    T_HANDLE(PixelSetAlphaQuantum, 0)     // PixelWand*
    T_NUMBER(PixelSetAlphaQuantum, 1)     // Quantum

    PixelSetAlphaQuantum((PixelWand *)(long)PARAM(0), (Quantum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetBlack, 2)
    T_HANDLE(PixelSetBlack, 0)     // PixelWand*
    T_NUMBER(PixelSetBlack, 1)     // double

    PixelSetBlack((PixelWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetBlackQuantum, 2)
    T_HANDLE(PixelSetBlackQuantum, 0)     // PixelWand*
    T_NUMBER(PixelSetBlackQuantum, 1)     // Quantum

    PixelSetBlackQuantum((PixelWand *)(long)PARAM(0), (Quantum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetBlue, 2)
    T_HANDLE(PixelSetBlue, 0)     // PixelWand*
    T_NUMBER(PixelSetBlue, 1)     // double

    PixelSetBlue((PixelWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetBlueQuantum, 2)
    T_HANDLE(PixelSetBlueQuantum, 0)     // PixelWand*
    T_NUMBER(PixelSetBlueQuantum, 1)     // Quantum

    PixelSetBlueQuantum((PixelWand *)(long)PARAM(0), (Quantum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetColorFromWand, 2)
    T_HANDLE(PixelSetColorFromWand, 0)     // PixelWand*
    T_HANDLE(PixelSetColorFromWand, 1)     // PixelWand*

    PixelSetColorFromWand((PixelWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetColorCount, 2)
    T_HANDLE(PixelSetColorCount, 0)     // PixelWand*
    T_NUMBER(PixelSetColorCount, 1)     // size_t

    PixelSetColorCount((PixelWand *)(long)PARAM(0), (size_t)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetCyan, 2)
    T_HANDLE(PixelSetCyan, 0)     // PixelWand*
    T_NUMBER(PixelSetCyan, 1)     // double

    PixelSetCyan((PixelWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetCyanQuantum, 2)
    T_HANDLE(PixelSetCyanQuantum, 0)     // PixelWand*
    T_NUMBER(PixelSetCyanQuantum, 1)     // Quantum

    PixelSetCyanQuantum((PixelWand *)(long)PARAM(0), (Quantum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetFuzz, 2)
    T_HANDLE(PixelSetFuzz, 0)     // PixelWand*
    T_NUMBER(PixelSetFuzz, 1)     // double

    PixelSetFuzz((PixelWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetGreen, 2)
    T_HANDLE(PixelSetGreen, 0)     // PixelWand*
    T_NUMBER(PixelSetGreen, 1)     // double

    PixelSetGreen((PixelWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetGreenQuantum, 2)
    T_HANDLE(PixelSetGreenQuantum, 0)     // PixelWand*
    T_NUMBER(PixelSetGreenQuantum, 1)     // Quantum

    PixelSetGreenQuantum((PixelWand *)(long)PARAM(0), (Quantum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetHSL, 4)
    T_HANDLE(PixelSetHSL, 0)     // PixelWand*
    T_NUMBER(PixelSetHSL, 1)     // double
    T_NUMBER(PixelSetHSL, 2)     // double
    T_NUMBER(PixelSetHSL, 3)     // double

    PixelSetHSL((PixelWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2), (double)PARAM(3));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetIndex, 2)
    T_HANDLE(PixelSetIndex, 0)     // PixelWand*
    T_NUMBER(PixelSetIndex, 1)     // IndexPacket

    PixelSetIndex((PixelWand *)(long)PARAM(0), (IndexPacket)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetMagenta, 2)
    T_HANDLE(PixelSetMagenta, 0)     // PixelWand*
    T_NUMBER(PixelSetMagenta, 1)     // double

    PixelSetMagenta((PixelWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetMagentaQuantum, 2)
    T_HANDLE(PixelSetMagentaQuantum, 0)     // PixelWand*
    T_NUMBER(PixelSetMagentaQuantum, 1)     // Quantum

    PixelSetMagentaQuantum((PixelWand *)(long)PARAM(0), (Quantum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetMagickColor, 11)
    T_HANDLE(PixelSetMagickColor, 0)     // PixelWand*
    T_NUMBER(PixelSetMagickColor, 1)
    T_NUMBER(PixelSetMagickColor, 2)
    T_NUMBER(PixelSetMagickColor, 3)
    T_NUMBER(PixelSetMagickColor, 4)
    T_NUMBER(PixelSetMagickColor, 5)
    T_NUMBER(PixelSetMagickColor, 6)
    T_NUMBER(PixelSetMagickColor, 7)
    T_NUMBER(PixelSetMagickColor, 8)
    T_NUMBER(PixelSetMagickColor, 9)
    T_NUMBER(PixelSetMagickColor, 10)

    MagickPixelPacket local_parameter_1;
    local_parameter_1.storage_class = (ClassType)PARAM(1);
    local_parameter_1.colorspace    = (ColorspaceType)PARAM(2);
    local_parameter_1.matte         = (MagickBooleanType)PARAM(3);
    local_parameter_1.fuzz          = PARAM(4);
    local_parameter_1.depth         = (size_t)PARAM(5);
    local_parameter_1.red           = (MagickRealType)PARAM(6);
    local_parameter_1.green         = (MagickRealType)PARAM(7);
    local_parameter_1.blue          = (MagickRealType)PARAM(8);
    local_parameter_1.opacity       = (MagickRealType)PARAM(9);
    local_parameter_1.index         = (MagickRealType)PARAM(10);

    PixelSetMagickColor((PixelWand *)(long)PARAM(0), &local_parameter_1);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetOpacity, 2)
    T_HANDLE(PixelSetOpacity, 0)     // PixelWand*
    T_NUMBER(PixelSetOpacity, 1)     // double

    PixelSetOpacity((PixelWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetOpacityQuantum, 2)
    T_HANDLE(PixelSetOpacityQuantum, 0)     // PixelWand*
    T_NUMBER(PixelSetOpacityQuantum, 1)     // Quantum

    PixelSetOpacityQuantum((PixelWand *)(long)PARAM(0), (Quantum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetQuantumColor, 5)
    T_HANDLE(PixelSetQuantumColor, 0)     // PixelWand*
    T_NUMBER(PixelSetQuantumColor, 1)
    T_NUMBER(PixelSetQuantumColor, 2)
    T_NUMBER(PixelSetQuantumColor, 3)
    T_NUMBER(PixelSetQuantumColor, 4)

    PixelPacket local_parameter_1;
    local_parameter_1.red     = (Quantum)PARAM(1);
    local_parameter_1.green   = (Quantum)PARAM(2);
    local_parameter_1.blue    = (Quantum)PARAM(3);
    local_parameter_1.opacity = (Quantum)PARAM(4);

    PixelSetQuantumColor((PixelWand *)(long)PARAM(0), &local_parameter_1);
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetRed, 2)
    T_HANDLE(PixelSetRed, 0)     // PixelWand*
    T_NUMBER(PixelSetRed, 1)     // double

    PixelSetRed((PixelWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetRedQuantum, 2)
    T_HANDLE(PixelSetRedQuantum, 0)     // PixelWand*
    T_NUMBER(PixelSetRedQuantum, 1)     // Quantum

    PixelSetRedQuantum((PixelWand *)(long)PARAM(0), (Quantum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetYellow, 2)
    T_HANDLE(PixelSetYellow, 0)     // PixelWand*
    T_NUMBER(PixelSetYellow, 1)     // double

    PixelSetYellow((PixelWand *)(long)PARAM(0), (double)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetYellowQuantum, 2)
    T_HANDLE(PixelSetYellowQuantum, 0)     // PixelWand*
    T_NUMBER(PixelSetYellowQuantum, 1)     // Quantum

    PixelSetYellowQuantum((PixelWand *)(long)PARAM(0), (Quantum)PARAM(1));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetFilename, 1)
    T_HANDLE(MagickGetFilename, 0)     // MagickWand*

    RETURN_STRING2((char *)MagickGetFilename((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetFormat, 1)
    T_HANDLE(MagickGetFormat, 0)     // MagickWand*

    RETURN_STRING2((char *)MagickGetFormat((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetFont, 1)
    T_HANDLE(MagickGetFont, 0)     // MagickWand*

    RETURN_STRING2((char *)MagickGetFont((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetHomeURL, 0)

    RETURN_STRING2((char *)MagickGetHomeURL())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageArtifact, 2)
    T_HANDLE(MagickGetImageArtifact, 0)     // MagickWand*
    T_STRING(MagickGetImageArtifact, 1)     // char*

    RETURN_STRING2((char *)MagickGetImageArtifact((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageArtifacts, 2)
    T_HANDLE(MagickGetImageArtifacts, 0)     // MagickWand*
    T_STRING(MagickGetImageArtifacts, 1)     // char*

    size_t local_parameter_2;

    char **ret = MagickGetImageArtifacts((MagickWand *)(long)PARAM(0), (char *)PARAM(1), &local_parameter_2);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_2; i++) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)ret[i], (double)0);
        IMAGICK_FREE_MEMORY(ret[i])
    }
    IMAGICK_FREE_MEMORY(ret)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageProfiles, 2)
    T_HANDLE(MagickGetImageProfiles, 0)     // MagickWand*
    T_STRING(MagickGetImageProfiles, 1)     // char*

// ... parameter 2 is by reference (size_t*)
    size_t local_parameter_2;

    char **ret = MagickGetImageProfiles((MagickWand *)(long)PARAM(0), (char *)PARAM(1), &local_parameter_2);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_2; i++) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)ret[i], (double)0);
        IMAGICK_FREE_MEMORY(ret[i])
    }
    IMAGICK_FREE_MEMORY(ret)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageProperty, 2)
    T_HANDLE(MagickGetImageProperty, 0)     // MagickWand*
    T_STRING(MagickGetImageProperty, 1)     // char*

    RETURN_STRING2((char *)MagickGetImageProperty((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageProperties, 2)
    T_HANDLE(MagickGetImageProperties, 0)     // MagickWand*
    T_STRING(MagickGetImageProperties, 1)     // char*

// ... parameter 2 is by reference (size_t*)
    size_t local_parameter_2;

    char **ret = MagickGetImageProperties((MagickWand *)(long)PARAM(0), (char *)PARAM(1), &local_parameter_2);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_2; i++) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)ret[i], (double)0);
        IMAGICK_FREE_MEMORY(ret[i])
    }
    IMAGICK_FREE_MEMORY(ret)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetOption, 2)
    T_HANDLE(MagickGetOption, 0)     // MagickWand*
    T_STRING(MagickGetOption, 1)     // char*

    RETURN_STRING2((char *)MagickGetOption((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetOptions, 2)
    T_HANDLE(MagickGetOptions, 0)     // MagickWand*
    T_STRING(MagickGetOptions, 1)     // char*

// ... parameter 2 is by reference (size_t*)
    size_t local_parameter_2;

    char **ret = MagickGetOptions((MagickWand *)(long)PARAM(0), (char *)PARAM(1), &local_parameter_2);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_2; i++) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)ret[i], (double)0);
        IMAGICK_FREE_MEMORY(ret[i])
    }
    IMAGICK_FREE_MEMORY(ret)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickQueryConfigureOption, 1)
    T_STRING(MagickQueryConfigureOption, 0)     // char*

    RETURN_STRING2((char *)MagickQueryConfigureOption((char *)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickQueryConfigureOptions, 1)
    T_STRING(MagickQueryConfigureOptions, 0)     // char*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;

    char **ret = MagickQueryConfigureOptions((char *)PARAM(0), &local_parameter_1);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_1; i++) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)ret[i], (double)0);
        IMAGICK_FREE_MEMORY(ret[i])
    }
    IMAGICK_FREE_MEMORY(ret)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickQueryFonts, 1)
    T_STRING(MagickQueryFonts, 0)     // char*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;

    char **ret = MagickQueryFonts((char *)PARAM(0), &local_parameter_1);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_1; i++) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)ret[i], (double)0);
        IMAGICK_FREE_MEMORY(ret[i])
    }
    IMAGICK_FREE_MEMORY(ret)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickQueryFormats, 1)
    T_STRING(MagickQueryFormats, 0)     // char*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;

    char **ret = MagickQueryFormats((char *)PARAM(0), &local_parameter_1);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_1; i++) {
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_STRING, (char *)ret[i], (double)0);
        IMAGICK_FREE_MEMORY(ret[i])
    }
    IMAGICK_FREE_MEMORY(ret)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetColorspace, 1)
    T_HANDLE(MagickGetColorspace, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetColorspace((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetCompression, 1)
    T_HANDLE(MagickGetCompression, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetCompression((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetCopyright, 0)

    RETURN_STRING2((char *)MagickGetCopyright())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetPackageName, 0)

    RETURN_STRING2((char *)MagickGetPackageName())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetQuantumDepth, 1)

// ... parameter 0 is by reference (size_t*)
    size_t local_parameter_0;

    RETURN_STRING2((char *)MagickGetQuantumDepth(&local_parameter_0))
    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetQuantumRange, 1)

// ... parameter 0 is by reference (size_t*)
    size_t local_parameter_0;

    RETURN_STRING2((char *)MagickGetQuantumRange(&local_parameter_0))
    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetReleaseDate, 0)

    RETURN_STRING2((char *)MagickGetReleaseDate())
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetVersion, 1)

// ... parameter 0 is by reference (size_t*)
    size_t local_parameter_0;

    RETURN_STRING2((char *)MagickGetVersion(&local_parameter_0))
    SET_NUMBER(0, (long)local_parameter_0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetPointsize, 1)
    T_HANDLE(MagickGetPointsize, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetPointsize((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetSamplingFactors, 1)
    T_HANDLE(MagickGetSamplingFactors, 0)     // MagickWand*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;

    double *ret = MagickGetSamplingFactors((MagickWand *)(long)PARAM(0), &local_parameter_1);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_1; i++)
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)ret[i]);
    IMAGICK_FREE_MEMORY(ret);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickQueryFontMetrics, 3)
    T_HANDLE(MagickQueryFontMetrics, 0)     // MagickWand*
    T_HANDLE(MagickQueryFontMetrics, 1)     // DrawingWand*
    T_STRING(MagickQueryFontMetrics, 2)     // char*

    double *ret = MagickQueryFontMetrics((MagickWand *)(long)PARAM(0), (DrawingWand *)(long)PARAM(1), (char *)PARAM(2));
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < 13; i++)
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)ret[i]);
    IMAGICK_FREE_MEMORY(ret);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickQueryMultilineFontMetrics, 3)
    T_HANDLE(MagickQueryMultilineFontMetrics, 0)     // MagickWand*
    T_HANDLE(MagickQueryMultilineFontMetrics, 1)     // DrawingWand*
    T_STRING(MagickQueryMultilineFontMetrics, 2)     // char*

    double *ret = MagickQueryMultilineFontMetrics((MagickWand *)(long)PARAM(0), (DrawingWand *)(long)PARAM(1), (char *)PARAM(2));
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < 13; i++)
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)ret[i]);
    IMAGICK_FREE_MEMORY(ret);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetGravity, 1)
    T_HANDLE(MagickGetGravity, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetGravity((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetType, 1)
    T_HANDLE(MagickGetType, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetType((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetInterlaceScheme, 1)
    T_HANDLE(MagickGetInterlaceScheme, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetInterlaceScheme((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetInterpolateMethod, 1)
    T_HANDLE(MagickGetInterpolateMethod, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetInterpolateMethod((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetOrientation, 1)
    T_HANDLE(MagickGetOrientation, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetOrientation((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDeleteImageArtifact, 2)
    T_HANDLE(MagickDeleteImageArtifact, 0)     // MagickWand*
    T_STRING(MagickDeleteImageArtifact, 1)     // char*

    RETURN_NUMBER(MagickDeleteImageArtifact((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDeleteImageProperty, 2)
    T_HANDLE(MagickDeleteImageProperty, 0)     // MagickWand*
    T_STRING(MagickDeleteImageProperty, 1)     // char*

    RETURN_NUMBER(MagickDeleteImageProperty((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickDeleteOption, 2)
    T_HANDLE(MagickDeleteOption, 0)     // MagickWand*
    T_STRING(MagickDeleteOption, 1)     // char*

    RETURN_NUMBER(MagickDeleteOption((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetAntialias, 1)
    T_HANDLE(MagickGetAntialias, 0)     // MagickWand*

    RETURN_NUMBER(MagickGetAntialias((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetPage, 5)
    T_HANDLE(MagickGetPage, 0)     // MagickWand*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;
// ... parameter 2 is by reference (size_t*)
    size_t local_parameter_2;
// ... parameter 3 is by reference (ssize_t*)
    ssize_t local_parameter_3;
// ... parameter 4 is by reference (ssize_t*)
    ssize_t local_parameter_4;

    RETURN_NUMBER(MagickGetPage((MagickWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2, &local_parameter_3, &local_parameter_4))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
    SET_NUMBER(3, (long)local_parameter_3)
    SET_NUMBER(4, (long)local_parameter_4)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetResolution, 3)
    T_HANDLE(MagickGetResolution, 0)     // MagickWand*

// ... parameter 1 is by reference (double*)
    double local_parameter_1;
// ... parameter 2 is by reference (double*)
    double local_parameter_2;

    RETURN_NUMBER(MagickGetResolution((MagickWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetSize, 3)
    T_HANDLE(MagickGetSize, 0)     // MagickWand*

// ... parameter 1 is by reference (size_t*)
    size_t local_parameter_1;
// ... parameter 2 is by reference (size_t*)
    size_t local_parameter_2;

    RETURN_NUMBER(MagickGetSize((MagickWand *)(long)PARAM(0), &local_parameter_1, &local_parameter_2))
    SET_NUMBER(1, (long)local_parameter_1)
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetSizeOffset, 2)
    T_HANDLE(MagickGetSizeOffset, 0)     // MagickWand*

// ... parameter 1 is by reference (ssize_t*)
    ssize_t local_parameter_1;

    RETURN_NUMBER(MagickGetSizeOffset((MagickWand *)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickProfileImage, 4)
    T_HANDLE(MagickProfileImage, 0)     // MagickWand*
    T_STRING(MagickProfileImage, 1)     // char*
    T_NUMBER(MagickProfileImage, 2)     // void*
    T_NUMBER(MagickProfileImage, 3)     // size_t

    RETURN_NUMBER(MagickProfileImage((MagickWand *)(long)PARAM(0), (char *)PARAM(1), (void *)(long)PARAM(2), (size_t)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetAntialias, 2)
    T_HANDLE(MagickSetAntialias, 0)     // MagickWand*
    T_NUMBER(MagickSetAntialias, 1)     // MagickBooleanType

    RETURN_NUMBER(MagickSetAntialias((MagickWand *)(long)PARAM(0), (MagickBooleanType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetBackgroundColor, 2)
    T_HANDLE(MagickSetBackgroundColor, 0)     // MagickWand*
    T_HANDLE(MagickSetBackgroundColor, 1)     // PixelWand*

    RETURN_NUMBER(MagickSetBackgroundColor((MagickWand *)(long)PARAM(0), (PixelWand *)(long)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetColorspace, 2)
    T_HANDLE(MagickSetColorspace, 0)     // MagickWand*
    T_NUMBER(MagickSetColorspace, 1)     // ColorspaceType

    RETURN_NUMBER(MagickSetColorspace((MagickWand *)(long)PARAM(0), (ColorspaceType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetCompression, 2)
    T_HANDLE(MagickSetCompression, 0)     // MagickWand*
    T_NUMBER(MagickSetCompression, 1)     // CompressionType

    RETURN_NUMBER(MagickSetCompression((MagickWand *)(long)PARAM(0), (CompressionType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetCompressionQuality, 2)
    T_HANDLE(MagickSetCompressionQuality, 0)     // MagickWand*
    T_NUMBER(MagickSetCompressionQuality, 1)     // size_t

    RETURN_NUMBER(MagickSetCompressionQuality((MagickWand *)(long)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetDepth, 2)
    T_HANDLE(MagickSetDepth, 0)     // MagickWand*
    T_NUMBER(MagickSetDepth, 1)     // size_t

    RETURN_NUMBER(MagickSetDepth((MagickWand *)(long)PARAM(0), (size_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetExtract, 2)
    T_HANDLE(MagickSetExtract, 0)     // MagickWand*
    T_STRING(MagickSetExtract, 1)     // char*

    RETURN_NUMBER(MagickSetExtract((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetFilename, 2)
    T_HANDLE(MagickSetFilename, 0)     // MagickWand*
    T_STRING(MagickSetFilename, 1)     // char*

    RETURN_NUMBER(MagickSetFilename((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetFormat, 2)
    T_HANDLE(MagickSetFormat, 0)     // MagickWand*
    T_STRING(MagickSetFormat, 1)     // char*

    RETURN_NUMBER(MagickSetFormat((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetFont, 2)
    T_HANDLE(MagickSetFont, 0)     // MagickWand*
    T_STRING(MagickSetFont, 1)     // char*

    RETURN_NUMBER(MagickSetFont((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetGravity, 2)
    T_HANDLE(MagickSetGravity, 0)     // MagickWand*
    T_NUMBER(MagickSetGravity, 1)     // GravityType

    RETURN_NUMBER(MagickSetGravity((MagickWand *)(long)PARAM(0), (GravityType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageArtifact, 3)
    T_HANDLE(MagickSetImageArtifact, 0)     // MagickWand*
    T_STRING(MagickSetImageArtifact, 1)     // char*
    T_STRING(MagickSetImageArtifact, 2)     // char*

    RETURN_NUMBER(MagickSetImageArtifact((MagickWand *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageProfile, 4)
    T_HANDLE(MagickSetImageProfile, 0)     // MagickWand*
    T_STRING(MagickSetImageProfile, 1)     // char*
    T_NUMBER(MagickSetImageProfile, 2)     // void*
    T_NUMBER(MagickSetImageProfile, 3)     // size_t

    RETURN_NUMBER(MagickSetImageProfile((MagickWand *)(long)PARAM(0), (char *)PARAM(1), (void *)(long)PARAM(2), (size_t)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetImageProperty, 3)
    T_HANDLE(MagickSetImageProperty, 0)     // MagickWand*
    T_STRING(MagickSetImageProperty, 1)     // char*
    T_STRING(MagickSetImageProperty, 2)     // char*

    RETURN_NUMBER(MagickSetImageProperty((MagickWand *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetInterlaceScheme, 2)
    T_HANDLE(MagickSetInterlaceScheme, 0)     // MagickWand*
    T_NUMBER(MagickSetInterlaceScheme, 1)     // InterlaceType

    RETURN_NUMBER(MagickSetInterlaceScheme((MagickWand *)(long)PARAM(0), (InterlaceType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetInterpolateMethod, 2)
    T_HANDLE(MagickSetInterpolateMethod, 0)     // MagickWand*
    T_NUMBER(MagickSetInterpolateMethod, 1)     // InterpolatePixelMethod

    RETURN_NUMBER(MagickSetInterpolateMethod((MagickWand *)(long)PARAM(0), (InterpolatePixelMethod)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetOption, 3)
    T_HANDLE(MagickSetOption, 0)     // MagickWand*
    T_STRING(MagickSetOption, 1)     // char*
    T_STRING(MagickSetOption, 2)     // char*

    RETURN_NUMBER(MagickSetOption((MagickWand *)(long)PARAM(0), (char *)PARAM(1), (char *)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetOrientation, 2)
    T_HANDLE(MagickSetOrientation, 0)     // MagickWand*
    T_NUMBER(MagickSetOrientation, 1)     // OrientationType

    RETURN_NUMBER(MagickSetOrientation((MagickWand *)(long)PARAM(0), (OrientationType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetPage, 5)
    T_HANDLE(MagickSetPage, 0)     // MagickWand*
    T_NUMBER(MagickSetPage, 1)     // size_t
    T_NUMBER(MagickSetPage, 2)     // size_t
    T_NUMBER(MagickSetPage, 3)     // ssize_t
    T_NUMBER(MagickSetPage, 4)     // ssize_t

    RETURN_NUMBER(MagickSetPage((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (ssize_t)PARAM(3), (ssize_t)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetPassphrase, 2)
    T_HANDLE(MagickSetPassphrase, 0)     // MagickWand*
    T_STRING(MagickSetPassphrase, 1)     // char*

    RETURN_NUMBER(MagickSetPassphrase((MagickWand *)(long)PARAM(0), (char *)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetPointsize, 2)
    T_HANDLE(MagickSetPointsize, 0)     // MagickWand*
    T_NUMBER(MagickSetPointsize, 1)     // double

    RETURN_NUMBER(MagickSetPointsize((MagickWand *)(long)PARAM(0), (double)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetResolution, 3)
    T_HANDLE(MagickSetResolution, 0)     // MagickWand*
    T_NUMBER(MagickSetResolution, 1)     // double
    T_NUMBER(MagickSetResolution, 2)     // double

    RETURN_NUMBER(MagickSetResolution((MagickWand *)(long)PARAM(0), (double)PARAM(1), (double)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetResourceLimit, 2)
    T_NUMBER(MagickSetResourceLimit, 0)     // ResourceType
    T_NUMBER(MagickSetResourceLimit, 1)     // MagickSizeType

    RETURN_NUMBER(MagickSetResourceLimit((ResourceType)PARAM(0), (MagickSizeType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetSamplingFactors, 3)
    T_HANDLE(MagickSetSamplingFactors, 0)     // MagickWand*
    T_NUMBER(MagickSetSamplingFactors, 1)     // size_t

// ... parameter 2 is by reference (double*)
    double local_parameter_2;

    RETURN_NUMBER(MagickSetSamplingFactors((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetSize, 3)
    T_HANDLE(MagickSetSize, 0)     // MagickWand*
    T_NUMBER(MagickSetSize, 1)     // size_t
    T_NUMBER(MagickSetSize, 2)     // size_t

    RETURN_NUMBER(MagickSetSize((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetSizeOffset, 4)
    T_HANDLE(MagickSetSizeOffset, 0)     // MagickWand*
    T_NUMBER(MagickSetSizeOffset, 1)     // size_t
    T_NUMBER(MagickSetSizeOffset, 2)     // size_t
    T_NUMBER(MagickSetSizeOffset, 3)     // ssize_t

    RETURN_NUMBER(MagickSetSizeOffset((MagickWand *)(long)PARAM(0), (size_t)PARAM(1), (size_t)PARAM(2), (ssize_t)PARAM(3)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickSetType, 2)
    T_HANDLE(MagickSetType, 0)     // MagickWand*
    T_NUMBER(MagickSetType, 1)     // ImageType

    RETURN_NUMBER(MagickSetType((MagickWand *)(long)PARAM(0), (ImageType)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------

/*CONCEPT_FUNCTION_IMPL(MagickSetProgressMonitor,3)
        T_HANDLE(MagickSetType, 0) // MagickWand*
        T_NUMBER(MagickSetType, 1) // MagickProgressMonitor
        T_NUMBER(MagickSetType, 2) // void*

        RETURN_NUMBER(MagickSetProgressMonitor((MagickWand*)(long)PARAM(0), (MagickProgressMonitor)PARAM(1), (void *)(long)PARAM(2)))
   END_IMPL*/
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetResource, 1)
    T_NUMBER(MagickGetResource, 0)     // ResourceType

    RETURN_NUMBER(MagickGetResource((ResourceType)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetResourceLimit, 1)
    T_NUMBER(MagickGetResourceLimit, 0)     // ResourceType

    RETURN_NUMBER(MagickGetResourceLimit((ResourceType)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetBackgroundColor, 1)
    T_HANDLE(MagickGetBackgroundColor, 0)     // MagickWand*

    RETURN_NUMBER((long)MagickGetBackgroundColor((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickGetImageProfile, 3)
    T_HANDLE(MagickGetImageProfile, 0)     // MagickWand*
    T_STRING(MagickGetImageProfile, 1)     // char*

// ... parameter 2 is by reference (size_t*)
    size_t local_parameter_2;

    RETURN_NUMBER((long)MagickGetImageProfile((MagickWand *)(long)PARAM(0), (char *)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(MagickRemoveImageProfile, 3)
    T_HANDLE(MagickRemoveImageProfile, 0)     // MagickWand*
    T_STRING(MagickRemoveImageProfile, 1)     // char*

// ... parameter 2 is by reference (size_t*)
    size_t local_parameter_2;

    RETURN_NUMBER((long)MagickRemoveImageProfile((MagickWand *)(long)PARAM(0), (char *)PARAM(1), &local_parameter_2))
    SET_NUMBER(2, (long)local_parameter_2)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetIteratorException, 2)
    T_HANDLE(PixelGetIteratorException, 0)     // PixelIterator*

// ... parameter 1 is by reference (ExceptionType*)
    ExceptionType local_parameter_1;

    RETURN_STRING((char *)PixelGetIteratorException((PixelIterator *)(long)PARAM(0), &local_parameter_1))
    SET_NUMBER(1, (long)local_parameter_1)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetIteratorExceptionType, 1)
    T_HANDLE(PixelGetIteratorExceptionType, 0)     // PixelIterator*

    RETURN_NUMBER(PixelGetIteratorExceptionType((PixelIterator *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(IsPixelIterator, 1)
    T_HANDLE(IsPixelIterator, 0)     // PixelIterator*

    RETURN_NUMBER(IsPixelIterator((PixelIterator *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelClearIteratorException, 1)
    T_HANDLE(PixelClearIteratorException, 0)     // PixelIterator*

    RETURN_NUMBER(PixelClearIteratorException((PixelIterator *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetIteratorRow, 2)
    T_HANDLE(PixelSetIteratorRow, 0)     // PixelIterator*
    T_NUMBER(PixelSetIteratorRow, 1)     // ssize_t

    RETURN_NUMBER(PixelSetIteratorRow((PixelIterator *)(long)PARAM(0), (ssize_t)PARAM(1)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSyncIterator, 1)
    T_HANDLE(PixelSyncIterator, 0)     // PixelIterator*

    RETURN_NUMBER(PixelSyncIterator((PixelIterator *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ClonePixelIterator, 1)
    T_HANDLE(ClonePixelIterator, 0)     // PixelIterator*

    RETURN_NUMBER((long)ClonePixelIterator((PixelIterator *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(DestroyPixelIterator, 1)
    T_HANDLE(DestroyPixelIterator, 0)     // PixelIterator*

    RETURN_NUMBER((long)DestroyPixelIterator((PixelIterator *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(NewPixelIterator, 1)
    T_HANDLE(NewPixelIterator, 0)     // MagickWand*

    RETURN_NUMBER((long)NewPixelIterator((MagickWand *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(NewPixelRegionIterator, 5)
    T_HANDLE(NewPixelRegionIterator, 0)     // MagickWand*
    T_NUMBER(NewPixelRegionIterator, 1)     // ssize_t
    T_NUMBER(NewPixelRegionIterator, 2)     // ssize_t
    T_NUMBER(NewPixelRegionIterator, 3)     // size_t
    T_NUMBER(NewPixelRegionIterator, 4)     // size_t

    RETURN_NUMBER((long)NewPixelRegionIterator((MagickWand *)(long)PARAM(0), (ssize_t)PARAM(1), (ssize_t)PARAM(2), (size_t)PARAM(3), (size_t)PARAM(4)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetCurrentIteratorRow, 1)
    T_HANDLE(PixelGetCurrentIteratorRow, 0)     // PixelIterator*

    size_t local_parameter_1;

    PixelWand **wand = PixelGetCurrentIteratorRow((PixelIterator *)(long)PARAM(0), &local_parameter_1);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_1; i++)
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)wand[i]);
    IMAGICK_FREE_MEMORY(wand);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetNextIteratorRow, 1)
    T_HANDLE(PixelGetNextIteratorRow, 0)     // PixelIterator*

    size_t local_parameter_1;

    PixelWand **wand = PixelGetNextIteratorRow((PixelIterator *)(long)PARAM(0), &local_parameter_1);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_1; i++)
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)wand[i]);
    IMAGICK_FREE_MEMORY(wand);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetPreviousIteratorRow, 1)
    T_HANDLE(PixelGetPreviousIteratorRow, 0)     // PixelIterator*

    size_t local_parameter_1;

    PixelWand **wand = PixelGetPreviousIteratorRow((PixelIterator *)(long)PARAM(0), &local_parameter_1);
    Invoke(INVOKE_CREATE_ARRAY, RESULT, 0);

    for (int i = 0; i < local_parameter_1; i++)
        Invoke(INVOKE_SET_ARRAY_ELEMENT, RESULT, (INTEGER)i, (INTEGER)VARIABLE_NUMBER, (char *)"", (double)(SYS_INT)wand[i]);
    IMAGICK_FREE_MEMORY(wand);
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelGetIteratorRow, 1)
    T_HANDLE(PixelGetIteratorRow, 0)     // PixelIterator*

    RETURN_NUMBER(PixelGetIteratorRow((PixelIterator *)(long)PARAM(0)))
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(ClearPixelIterator, 1)
    T_HANDLE(ClearPixelIterator, 0)     // PixelIterator*

    ClearPixelIterator((PixelIterator *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelResetIterator, 1)
    T_HANDLE(PixelResetIterator, 0)     // PixelIterator*

    PixelResetIterator((PixelIterator *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetFirstIteratorRow, 1)
    T_HANDLE(PixelSetFirstIteratorRow, 0)     // PixelIterator*

    PixelSetFirstIteratorRow((PixelIterator *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL
//------------------------------------------------------------------------
CONCEPT_FUNCTION_IMPL(PixelSetLastIteratorRow, 1)
    T_HANDLE(PixelSetLastIteratorRow, 0)     // PixelIterator*

    PixelSetLastIteratorRow((PixelIterator *)(long)PARAM(0));
    RETURN_NUMBER(0)
END_IMPL

