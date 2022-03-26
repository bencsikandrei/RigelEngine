/* Copyright (C) 2019, Nikolai Wuttke. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "upscaling_utils.hpp"

#include "base/math_utils.hpp"
#include "data/game_options.hpp"
#include "data/game_traits.hpp"
#include "renderer/renderer.hpp"
#include "renderer/viewport_utils.hpp"

#include <algorithm>


namespace rigel::renderer
{

namespace
{

base::Size<float>
  determineUsableSize(const float windowWidth, const float windowHeight)
{
  auto quantize = [](const float value) {
    return float(int(value) - int(value) % 8);
  };

  const auto actualAspectRatioIsWiderThanTarget =
    windowWidth / windowHeight > data::GameTraits::aspectRatio;
  if (actualAspectRatioIsWiderThanTarget)
  {
    const auto evenHeight = quantize(windowHeight);
    return {data::GameTraits::aspectRatio * evenHeight, evenHeight};
  }
  else
  {
    return {
      quantize(windowWidth),
      quantize(1.0f / data::GameTraits::aspectRatio * windowWidth)};
  }
}


int determineLowResBufferWidth(
  const Renderer* pRenderer,
  const bool widescreenModeWanted)
{
  if (widescreenModeWanted && canUseWidescreenMode(pRenderer))
  {
    const auto scale = determineViewport(pRenderer).mScale.x;
    const auto fullWidth = determineWidescreenViewport(pRenderer).mWidthPx;
    return base::round(fullWidth / scale);
  }

  return data::GameTraits::viewportWidthPx;
}

} // namespace


ViewportInfo determineViewport(const Renderer* pRenderer)
{
  const auto windowWidth = float(pRenderer->windowSize().width);
  const auto windowHeight = float(pRenderer->windowSize().height);

  const auto [usableWidth, usableHeight] =
    determineUsableSize(windowWidth, windowHeight);

  const auto widthScale = usableWidth / data::GameTraits::viewportWidthPx;
  const auto heightScale = usableHeight / data::GameTraits::viewportHeightPx;
  const auto offsetX = (windowWidth - usableWidth) / 2.0f;
  const auto offsetY = (windowHeight - usableHeight) / 2.0f;

  return {
    base::Vec2{int(offsetX), int(offsetY)},
    base::Size<int>{int(usableWidth), int(usableHeight)},
    base::Vec2f{widthScale, heightScale}};
}


bool canUseWidescreenMode(const Renderer* pRenderer)
{
  const auto windowWidth = float(pRenderer->windowSize().width);
  const auto windowHeight = float(pRenderer->windowSize().height);
  return windowWidth / windowHeight > data::GameTraits::aspectRatio;
}


WidescreenViewportInfo determineWidescreenViewport(const Renderer* pRenderer)
{
  const auto info = determineViewport(pRenderer);

  const auto windowWidth = pRenderer->windowSize().width;
  const auto tileWidthScaled = data::GameTraits::tileSize * info.mScale.x;
  const auto maxTilesOnScreen = int(windowWidth / tileWidthScaled);

  const auto widthInPixels =
    std::min(base::round(maxTilesOnScreen * tileWidthScaled), windowWidth);
  const auto paddingPixels = pRenderer->windowSize().width - widthInPixels;

  return {maxTilesOnScreen, widthInPixels, paddingPixels / 2};
}


RenderTargetTexture createFullscreenRenderTarget(
  Renderer* pRenderer,
  const data::GameOptions& options)
{
  if (options.mPerElementUpscalingEnabled)
  {
    return RenderTargetTexture{
      pRenderer, pRenderer->windowSize().width, pRenderer->windowSize().height};
  }
  else
  {
    return RenderTargetTexture{
      pRenderer,
      determineLowResBufferWidth(pRenderer, options.mWidescreenModeOn),
      data::GameTraits::viewportHeightPx};
  }
}


base::Vec2 offsetTo4by3WithinWidescreen(
  Renderer* pRenderer,
  const data::GameOptions& options)
{
  const auto viewportInfo = determineViewport(pRenderer);
  if (options.mPerElementUpscalingEnabled)
  {
    return viewportInfo.mOffset;
  }

  return scaleVec(
    viewportInfo.mOffset,
    {1.0f / viewportInfo.mScale.x, 1.0f / viewportInfo.mScale.y});
}

} // namespace rigel::renderer
