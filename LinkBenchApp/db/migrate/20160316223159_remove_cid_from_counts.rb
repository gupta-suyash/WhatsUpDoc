class RemoveCidFromCounts < ActiveRecord::Migration
  def change
	remove_column :counts, :id1, :integer
  end
end
