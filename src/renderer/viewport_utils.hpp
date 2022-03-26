/* Copyright (C) 2022, Nikolai Wuttke. All rights reserved.
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

#pragma once

#include "base/spatial_types.hpp"


namespace rigel::renderer
{
class Renderer;
}


namespace rigel::renderer
{

base::Vec2 scaleVec(const base::Vec2& vec, const base::Vec2f& scale);
base::Extents scaleSize(const base::Extents& size, const base::Vec2f& scale);

void setLocalTranslation(
  renderer::Renderer* pRenderer,
  const base::Vec2& translation);
void setLocalClipRect(
  renderer::Renderer* pRenderer,
  const base::Rect<int>& localRect);

} // namespace rigel::renderer
