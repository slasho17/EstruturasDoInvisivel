import numpy as np
import matplotlib.pyplot as plt
from shapely.geometry import Point, Polygon
from shapely.ops import unary_union

def create_regular_polygon(n_sides, radius, center=(0, 0), start_angle=0):
    """
    Create a regular polygon as a Shapely Polygon.
    """
    cx, cy = center
    angles = np.linspace(0, 360, n_sides, endpoint=False) + start_angle
    points = []
    for angle in angles:
        rad = np.radians(angle)
        x = cx + radius * np.cos(rad)
        y = cy + radius * np.sin(rad)
        points.append((x, y))
    return Polygon(points)

def polygon_side_midpoints(polygon):
    """
    Return midpoints and outward normals for each side of a polygon.
    """
    coords = list(polygon.exterior.coords)
    side_info = []
    for i in range(len(coords) - 1):
        x1, y1 = coords[i]
        x2, y2 = coords[i + 1]
        
        midx = (x1 + x2) / 2
        midy = (y1 + y2) / 2
        
        dx = x2 - x1
        dy = y2 - y1
        
        normal_angle = np.degrees(np.arctan2(dy, dx)) - 90
        side_info.append(((midx, midy), normal_angle))
    return side_info

def create_coverage_wedge(sensor_pos, sensor_angle, fov_degs, max_range, resolution=64):
    """
    Create a wedge (sector) as a Shapely Polygon for sensor coverage.
    """
    x0, y0 = sensor_pos
    half_fov = fov_degs / 2
    start_angle = sensor_angle - half_fov
    end_angle = sensor_angle + half_fov
    
    angles = np.linspace(start_angle, end_angle, resolution)
    arc_points = [(x0, y0)]
    for angle in angles:
        rad = np.radians(angle)
        x = x0 + max_range * np.cos(rad)
        y = y0 + max_range * np.sin(rad)
        arc_points.append((x, y))
    arc_points.append((x0, y0))
    
    return Polygon(arc_points)

def main():
    # --- PARAMETERS ---
    n_sides = 10
    # Set the polygon “radius” so that total diameter is 14 cm => 0.14 m
    polygon_radius = 0.07      # 7 cm in meters

    sensor_fov = 15            # degrees Field Of View
    sensor_range = 2.0         # in meters (still using 2m typical range)
    
    # Person parameters
    # 50 cm diameter => 25 cm radius => 0.25 m
    person_radius = 0.25
    # Person positioned 77 cm (0.77 m) from the center, along x-axis
    person_position = (0.77, 0.0)
    
    # 1. Create octagon
    octagon = create_regular_polygon(
        n_sides=n_sides,
        radius=polygon_radius,
        center=(0, 0),
        start_angle=0
    )
    
    # 2. Side midpoints + outward normals
    side_info = polygon_side_midpoints(octagon)
    
    # 3. Sensor coverage polygons
    coverage_polygons = []
    for (midpoint, normal_angle) in side_info:
        wedge_poly = create_coverage_wedge(
            sensor_pos=midpoint,
            sensor_angle=normal_angle,
            fov_degs=sensor_fov,
            max_range=sensor_range,
            resolution=64
        )
        coverage_polygons.append(wedge_poly)
    
    # 4. Combine coverage polygons
    total_coverage = unary_union(coverage_polygons)
    
    # 5. Person shape
    person_shape = Point(person_position).buffer(person_radius, resolution=64)
    
    # 6. Check detection
    person_detected = total_coverage.intersects(person_shape)
    print(f"Person at {person_position} {'IS' if person_detected else 'is NOT'} detected.")
    
    # 7. Plot
    fig, ax = plt.subplots(figsize=(6, 6))
    
    # Plot octagon
    x_oct, y_oct = octagon.exterior.xy
    ax.fill(x_oct, y_oct, alpha=0.2, fc='gray', ec='black', label='Polygon')
    
    # Plot coverage wedges
    for i, wedge_poly in enumerate(coverage_polygons):
        x_cov, y_cov = wedge_poly.exterior.xy
        ax.fill(x_cov, y_cov, alpha=0.2, label='Sensor Coverage' if i == 0 else "")
    
    # Plot sensor positions + orientation
    for i, (midpoint, normal_angle) in enumerate(side_info):
        ax.plot(midpoint[0], midpoint[1], 'ro')
        orientation_len = 0.02  # slightly shorter in new scale
        rad = np.radians(normal_angle)
        x2 = midpoint[0] + orientation_len * np.cos(rad)
        y2 = midpoint[1] + orientation_len * np.sin(rad)
        ax.plot([midpoint[0], x2], [midpoint[1], y2], 'r-')
        ax.text(midpoint[0], midpoint[1], f"S{i+1}", color='red', fontsize=8)
    
    # Plot person
    x_p, y_p = person_shape.exterior.xy
    ax.fill(x_p, y_p, alpha=0.5, fc='blue', label='Person')
    
    # Formatting
    ax.set_aspect('equal', 'box')
    # Adjust axes to show everything properly
    ax.set_xlim(-1, 2)  
    ax.set_ylim(-1, 1)
    ax.set_xlabel("X (m)")
    ax.set_ylabel("Y (m)")
    ax.set_title("Ultrasonic Sensor Coverage with " + str(n_sides) + " sides")
    ax.legend(loc='upper right')
    
    plt.savefig("sensor_coverage_" + str(n_sides) + "_sides.png", dpi=300)


if __name__ == "__main__":
    main()
